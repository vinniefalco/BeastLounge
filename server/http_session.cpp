//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "listener.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "session.hpp"
#include "utility.hpp"
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/yield.hpp>
#include <boost/optional.hpp>
#include <iostream>

extern
void
run_ws_session(
    server& srv,
    listener& lst,
    stream_type stream,
    endpoint_type ep,
    websocket::request_type req);

extern
void
run_ws_session(
    server& srv,
    listener& lst,
    beast::ssl_stream<
        stream_type> stream,
    endpoint_type ep,
    websocket::request_type req);

namespace {

// Return a reasonable mime type based on the extension of a file.
beast::string_view
mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
    beast::string_view base,
    beast::string_view path)
{
    if(base.empty())
        return path.to_string();
    std::string result = base.to_string();
#if BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
    class Body, class Allocator,
    class Send>
void
handle_request(
    beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send)
{
    // Returns a bad request response
    auto const bad_request =
    [&req](beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = why.to_string();
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found =
    [&req](beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + target.to_string() + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error =
    [&req](beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + what.to_string() + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if(req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if(ec == boost::system::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if(ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if(req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}

//------------------------------------------------------------------------------

template<class Derived>
class http_session_base
    : public asio::coroutine
    , public session
{
protected:
    server& srv_;
    listener& lst_;
    section& log_;
    endpoint_type ep_;
    flat_storage storage_;
    boost::optional<
        http::request_parser<
            http::string_body>> pr_;

public:
    http_session_base(
        server& srv,
        listener& lst,
        endpoint_type ep,
        flat_storage storage)
        : srv_(srv)
        , lst_(lst)
        , log_(srv_.log().get_section("http_session"))
        , ep_(ep)
        , storage_(std::move(storage))
    {
        lst_.insert(this);
    }

    ~http_session_base()
    {
        lst_.erase(this);
    }

    // The CRTP pattern
    Derived*
    impl()
    {
        return static_cast<Derived*>(this);
    }

    //--------------------------------------------------------------------------
    //
    // session
    //
    //--------------------------------------------------------------------------

    void
    on_stop() override
    {
        net::post(
            impl()->stream().get_executor(),
            bind_front(this, &http_session_base::do_stop));
    }

    void
    do_stop()
    {
        beast::error_code ec;
        beast::close_socket(
            beast::get_lowest_layer(impl()->stream()));
    }

    //--------------------------------------------------------------------------
    //
    // http_session
    //
    //--------------------------------------------------------------------------

    // We only require C++11, this helper is
    // the equivalent of a C++14 generic lambda.
    struct send_lambda
    {
        http_session_base& self_;

        template<bool isRequest, class Body, class Fields>
        void
        operator()(http::message<isRequest, Body, Fields>&& msg) const
        {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            auto sp = std::make_shared<
                http::message<isRequest, Body, Fields>>(
                    std::move(msg));

            // Write the response
            auto self = bind_front(&self_);
            http::async_write(
                self_.impl()->stream(),
                *sp,
                [self, sp](
                    beast::error_code ec,
                    std::size_t bytes_transferred)
                {
                    self(
                        ec,
                        bytes_transferred,
                        sp->need_eof());
                });
        }
    };

    void
    operator()(
        beast::error_code ec = {},
        std::size_t bytes_transferred = 0,
        bool need_eof = false)
    {
        boost::ignore_unused(bytes_transferred);
        reenter(*this)
        {
            // Set the expiration
            impl()->expires_after(std::chrono::seconds(30));

            // A new HTTP parser is required for each message
            pr_.emplace();

            // Set some limits to discourage attackers.
            pr_->body_limit(64 * 1024);
            pr_->header_limit(2048);

            // Read the next HTTP request
            yield http::async_read(
                impl()->stream(),
                storage_,
                *pr_,
                bind_front(this));

            // This means they closed the connection
            if(ec == http::error::end_of_stream)
            {
                return impl()->do_close();
            }

            // Handle the error, if any
            if(ec)
                return impl()->fail(ec, "http::async_read");

            // See if it is a WebSocket Upgrade
            if(websocket::is_upgrade(pr_->get()))
            {
                // Turn off the expiration timer
                impl()->expires_never();

                // Convert the request type
                websocket::request_type req(pr_->release());

                // Create a WebSocket session by transferring the socket
                return run_ws_session(
                    srv_, lst_,
                    std::move(impl()->stream()),
                    ep_,
                    std::move(req));
            }

            // Send the response
            yield
            handle_request(
                srv_.doc_root(),
                pr_->release(),
                send_lambda{*this});

            // Handle the error, if any
            if(ec)
                return impl()->fail(ec, "http::async_write");

            if(need_eof)
            {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                return impl()->do_close();
            }
        }
    }

};

//------------------------------------------------------------------------------

class plain_http_session_impl
    : public http_session_base<plain_http_session_impl>
{
    stream_type stream_;

public:
    plain_http_session_impl(
        server& srv,
        listener& lst,
        stream_type stream,
        endpoint_type ep,
        flat_storage storage)
        : http_session_base(
            srv, lst, ep, std::move(storage))
        , stream_(std::move(stream))
    {
    }

    stream_type&
    stream()
    {
        return stream_;
    }

    void
    expires_after(
        std::chrono::seconds n)
    {
        stream_.expires_after(n);
    }

    void
    expires_never()
    {
        stream_.expires_never();
    }

    void
    run()
    {
        // Use post to get on to our strand.
        net::post(
            stream_.get_executor(),
            bind_front(this));
    }

    void
    do_close()
    {
        beast::error_code ec;
        stream_.socket().shutdown(
            tcp::socket::shutdown_send, ec);
    }

    // Report a failure
    void
    fail(beast::error_code ec, char const* what)
    {
        if(ec == net::error::operation_aborted)
            LOG_TRC(log_, what, '\t', ec.message());
        else
            LOG_INF(log_, what, '\t', ec.message());
    }
};

//------------------------------------------------------------------------------

class ssl_http_session_impl
    : public http_session_base<ssl_http_session_impl>
{
    asio::ssl::context& ctx_;
    beast::ssl_stream<
        stream_type> stream_;

public:
    ssl_http_session_impl(
        server& srv,
        listener& lst,
        asio::ssl::context& ctx,
        stream_type stream,
        endpoint_type ep,
        flat_storage storage)
        : http_session_base(
            srv, lst, ep, std::move(storage))
        , ctx_(ctx)
        , stream_(std::move(stream), ctx)
    {
    }

    beast::ssl_stream<stream_type>&
    stream()
    {
        return stream_;
    }

    void
    expires_after(
        std::chrono::seconds n)
    {
        stream_.next_layer().expires_after(n);
    }

    void
    expires_never()
    {
        stream_.next_layer().expires_never();
    }

    void
    run()
    {
        // Use post to get on to our strand.
        net::post(
            stream_.get_executor(),
            bind_front(this,
                &ssl_http_session_impl::do_run));
    }

    void
    do_run()
    {
        // Set the expiration
        impl()->expires_after(std::chrono::seconds(30));

        // Perform the TLS handshake in the server role
        stream_.async_handshake(
            asio::ssl::stream_base::server,
            storage_.data(),
            bind_front(this, &ssl_http_session_impl::on_handshake));
    }

    void
    on_handshake(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        // Adjust the buffer for what the handshake used
        storage_.consume(bytes_transferred);

        // Report the error if any
        if(ec)
            return fail(ec, "async_handshake");

        // Process HTTP
        (*this)();
    }

    void
    do_close()
    {
        // Set the expiration
        expires_after(std::chrono::seconds(30));

        // Perform the TLS closing handshake
        stream_.async_shutdown(
            bind_front(this, &ssl_http_session_impl::on_shutdown));
    }

    void
    on_shutdown(beast::error_code ec)
    {
        if(ec)
            return fail(ec, "async_shutdown");
    }

    // Report a failure
    void
    fail(beast::error_code ec, char const* what)
    {
        // ssl::error::stream_truncated, also known as an SSL "short read",
        // indicates the peer closed the connection without performing the
        // required closing handshake (for example, Google does this to
        // improve performance). Generally this can be a security issue,
        // but if your communication protocol is self-terminated (as
        // it is with both HTTP and WebSocket) then you may simply
        // ignore the lack of close_notify.
        //
        // https://github.com/boostorg/beast/issues/38
        //
        // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
        //
        // When a short read would cut off the end of an HTTP message,
        // Beast returns the error beast::http::error::partial_message.
        // Therefore, if we see a short read here, it has occurred
        // after the message has been completed, so it is safe to ignore it.

        if(ec == asio::ssl::error::stream_truncated)
            return;

        if(ec == net::error::operation_aborted)
            LOG_TRC(log_, what, '\t', ec.message());
        else
            LOG_INF(log_, what, '\t', ec.message());
    }
};

} // (anon)

//------------------------------------------------------------------------------

void
run_http_session(
    server& srv,
    listener& lst,
    stream_type stream,
    endpoint_type ep,
    flat_storage storage)
{
    auto sp = boost::make_shared<
            plain_http_session_impl>(
        srv, lst,
        std::move(stream),
        ep,
        std::move(storage));
    sp->run();
}

void
run_https_session(
    server& srv,
    listener& lst,
    asio::ssl::context& ctx,
    stream_type stream,
    endpoint_type ep,
    flat_storage storage)
{
    auto sp = boost::make_shared<
            ssl_http_session_impl>(
        srv, lst, ctx,
        std::move(stream),
        ep,
        std::move(storage));
    sp->run();
}
