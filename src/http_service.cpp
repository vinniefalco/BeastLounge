//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "http_service.hpp"
#include "any_connection.hpp"
#include "http_connection.hpp"
#include "listener.hpp"
#include <boost/asio/coroutine.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/json/number_cast.hpp>
#include <string>
#include <utility>
#include <vector>

#include <lounge/bind_mf.hpp>

#define BEAST_LOUNGE_VERSION_STRING "BeastLounge/1.0b"

namespace lounge {

namespace {

//----------------------------------------------------------

class http_service_impl
    : public http_service
    , public listener::handler
{
    server& srv_;
    log& log_;
    std::vector<
        std::unique_ptr<listener>> v_;
    std::unique_ptr<
        any_connection::list> list_;
    std::string doc_root_;

public:
    using key_type = http_service;

    http_service_impl(
        server& srv)
        : srv_(srv)
        , log_(srv.get_log("http"))
        , list_(any_connection::list::create())
    {
        install_rpcs();
    }

    ~http_service_impl()
    {
    }

    void
    on_start() override
    {
        for(auto& p : v_)
            p->on_start();
    }

    void
    on_stop() override
    {
        LOG_TRC(log_, "on_stop");

        list_->close();

        for(auto& p : v_)
            p->on_stop();
    }

    // Called for incoming connections
    void
    on_accept(
        socket_type sock)
    {
        LOG_TRC(log_, "on_accept");

        create_http_connection(
            *list_,
            srv_,
            std::move(sock));
    }

    //------------------------------------------------------

    // Return a reasonable mime type based on the extension of a file.
    string_view
    mime_type(string_view path)
    {
        using beast::iequals;
        auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if(pos == string_view::npos)
                return string_view{};
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
        string_view base,
        string_view path)
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
    
    http_generator
    handle_request(
        http::request<
            http::string_body>&& req) override
    {
        string_view doc_root = doc_root_;

        // Returns a bad request response
        auto const bad_request =
        [&req](string_view why)
        {
            http::response<http::string_body> res{http::status::bad_request, req.version()};
            res.set(http::field::server, BEAST_LOUNGE_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = why.to_string();
            res.prepare_payload();
            return res;
        };

        // Returns a not found response
        auto const not_found =
        [&req](string_view target)
        {
            http::response<http::string_body> res{http::status::not_found, req.version()};
            res.set(http::field::server, BEAST_LOUNGE_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + target.to_string() + "' was not found.";
            res.prepare_payload();
            return res;
        };

        // Returns a server error response
        auto const server_error =
        [&req](string_view what)
        {
            http::response<http::string_body> res{http::status::internal_server_error, req.version()};
            res.set(http::field::server, BEAST_LOUNGE_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + what.to_string() + "'";
            res.prepare_payload();
            return res;
        };

        // Make sure we can handle the method
        if( req.method() != http::verb::get &&
            req.method() != http::verb::head)
            return bad_request("Unknown HTTP-method");

        // Request path must be absolute and not contain "..".
        if( req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != string_view::npos)
            return bad_request("Illegal request-target");

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
            return not_found(req.target());

        // Handle an unknown error
        if(ec)
            return server_error(ec.message());

        // Cache the size since we need it after the move
        auto const size = body.size();

        // Respond to HEAD request
        if(req.method() == http::verb::head)
        {
            http::response<http::empty_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BEAST_LOUNGE_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }

        // Respond to GET request
        http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, BEAST_LOUNGE_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    //------------------------------------------------------

    void
    install_rpcs()
    {
        srv_.add_rpc("http.create_port", bind_mf(&http_service_impl::do_create_port, this));
        srv_.add_rpc("http.set_doc_root", bind_mf(&http_service_impl::do_set_doc_root, this));
    }

    void
    do_create_port(
        rpc_response& res,
        json::value const& params)
    {
        auto addr = net::ip::make_address(
            params.at("address").as_string().c_str());
        auto port = json::number_cast<unsigned short>(
            params.at("port"));
        v_.emplace_back(
            listener::create(
                srv_,
                *this,
                tcp::endpoint(addr, port)));
    }

    void
    do_set_doc_root(
        rpc_response& res,
        json::value const& params)
    {
// VFALCO TEMPORARY
doc_root_ =
    "C:\\Users\\vinnie\\src\\lounge\\static\\wwwroot";
    }

    //------------------------------------------------------
};

} // (anon)

void
http_service::
install(
    server& srv)
{
    emplace_service<http_service_impl>(srv);
}

} // lounge
