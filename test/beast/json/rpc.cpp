//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/rpc.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <memory>

namespace boost {
namespace beast {
namespace json {

class rpc_test : public unit_test::suite
{
public:
    void check(rpc_error e)
    {
        auto const ec = make_error_code(e);
        ec.category().name();
        BEAST_EXPECT(! ec.message().empty());
    }

    void check(condition c, rpc_error e)
    {
        {
            auto const ec = make_error_code(e);
            BEAST_EXPECT(ec.category().name() != nullptr);
            BEAST_EXPECT(! ec.message().empty());
            BEAST_EXPECT(ec == c);
        }
        {
            auto ec = make_error_condition(c);
            BEAST_EXPECT(ec.category().name() != nullptr);
            BEAST_EXPECT(! ec.message().empty());
            BEAST_EXPECT(ec == c);
        }
    }

    void
    testErrors()
    {        
        check(rpc_error::parse_error);
        check(rpc_error::invalid_request);
        check(rpc_error::method_not_found);
        check(rpc_error::invalid_params);
        check(rpc_error::internal_error);
    }

    void
    testRequest()
    {
        {
            storage_ptr sp =
                make_storage_ptr(
                    std::allocator<char>{});
            value jv({
                { "jsonrpc", "2.0" },
                { "method", "test" }
            }, sp);
            auto req = rpc_request::extract(
                std::move(jv));
            BEAST_EXPECT(req.has_value());
            BEAST_EXPECT(
                req->method.get_allocator() ==
                allocator<char>(jv.get_storage()));
            BEAST_EXPECT(
                *req->params.get_storage() ==
                *jv.get_storage());
            BEAST_EXPECT(
                *req->id.get_storage() ==
                *jv.get_storage());
        }
    }

    void
    run() override
    {
        testErrors();
        testRequest();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,rpc);

} // json
} // beast
} // boost
