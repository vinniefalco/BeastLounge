//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "http_session.hpp"
#include "session.hpp"
#include "websocket_session.hpp"
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/yield.hpp>
#include <iostream>

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

struct http_session_impl
    : public boost::enable_shared_from_this<http_session_impl>
    , public asio::coroutine
    , public http_session
    , public session
{
    server& srv_;
    agent& ag_;
    logger::section& log_;
    stream_type stream_;
    stream_type::endpoint_type ep_;
    flat_storage storage_;
    http::request<http::string_body> req_;

    http_session_impl(
        server& srv,
        agent& ag,
        stream_type stream,
        stream_type::endpoint_type ep,
        flat_storage storage)
        : srv_(srv)
        , ag_(ag)
        , log_(srv_.log().get_section("http_session"))
        , stream_(std::move(stream))
        , ep_(ep)
        , storage_(std::move(storage))
    {
        ag_.insert(this);
    }

    ~http_session_impl()
    {
        ag_.remove(this);
    }

    void
    run() override
    {
        net::post(stream_.get_executor(), self(this));
    }

    // We only require C++11, this helper is
    // the equivalent of a C++14 generic lambda.
    struct send_lambda
    {
        http_session_impl& self_;

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
            auto self = self_.shared_from_this();
            http::async_write(
                self_.stream_,
                *sp,
                [self, sp](
                    beast::error_code ec,
                    std::size_t bytes_transferred)
                {
                    (*self)(
                        ec,
                        bytes_transferred,
                        sp->need_eof());
                });
        }
    };

    template<class Body, class Allocator>
    bool
    is_json_rpc(http::request<
        Body, http::basic_fields<Allocator>> const& req)
    {
        return
            req.method() == http::verb::get &&
            req.target() == "/api/http";
    }

    template<class Body, class Allocator>
    void
    do_json_rpc(http::request<
        Body, http::basic_fields<Allocator>> const& req)
    {
        json::value jv;
        srv_.stat(jv);
        http::response<http::string_body> res;
        res.version(req.version());
        res.result(http::status::ok);
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "application/json");
        std::stringstream ss;
        ss << jv;
        res.body() = ss.str();
        res.prepare_payload();

        // Write the message asynchronously
        send_lambda{*this}(std::move(res));
    }

    void
    operator()(
        beast::error_code ec = {},
        std::size_t bytes_transferred = 0,
        bool need_eof = false)
    {
        boost::ignore_unused(bytes_transferred);
        reenter(*this)
        {
            // Read the next HTTP request
            yield http::async_read(
                stream_,
                storage_,
                req_,
                self(this));

            // This means they closed the connection
            if(ec == http::error::end_of_stream)
            {
                stream_.socket().shutdown(
                    tcp::socket::shutdown_send, ec);
                return;
            }

            // Handle the error, if any
            if(ec)
                return fail(ec, "http::async_read");

            // See if it is a WebSocket Upgrade
            if(websocket::is_upgrade(req_))
            {
                // Create a WebSocket session by transferring the socket
                // ...
            }

            // See if it is a JSON-RPC request
            if(is_json_rpc(req_))
            {
                // Process it
                do_json_rpc(req_);
            }
            else
            {
                // Send the response
                yield
                handle_request(
                    srv_.doc_root(),
                    std::move(req_),
                    send_lambda{*this});
            }

            // Clear contents of the request message,
            // otherwise the read behavior is undefined.
            req_ = {};

            // Handle the error, if any
            if(ec)
                return fail(ec, "http::async_write");

            if(need_eof)
            {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.

                stream_.socket().shutdown(
                    tcp::socket::shutdown_send, ec);
                return;
            }
        }
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

    //--------------------------------------------------------------------------
    //
    // session
    //

    boost::weak_ptr<session>
    get_weak_ptr() override
    {
        return this->weak_from_this();
    }

    void
    on_stop() override
    {
        net::post(
            stream_.get_executor(),
            beast::bind_front_handler(
                &http_session_impl::do_stop,
                shared_from_this()));
    }

    void
    do_stop()
    {
        beast::error_code ec;
        beast::close_socket(
            beast::get_lowest_layer(stream_));
    }
};

} // (anon)

//------------------------------------------------------------------------------

boost::shared_ptr<http_session>
make_http_session(
    server& srv,
    agent& ag,
    stream_type stream,
    stream_type::endpoint_type ep,
    flat_storage storage)
{
    return boost::make_shared<http_session_impl>(
        srv,
        ag,
        std::move(stream),
        ep,
        std::move(storage));
}
