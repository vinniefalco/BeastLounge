//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "rpc.hpp"
#include "server.hpp"
#include "service.hpp"
#include "types.hpp"
#include "user.hpp"
#include <boost/make_unique.hpp>
#include <vector>
#include <utility>

namespace {

//------------------------------------------------------------------------------

struct shoe
{
    std::vector<char> cards;
    std::vector<char>::iterator pos;

    explicit
    shoe(int decks)
    {
        cards.resize(decks * 52);
        shuffle();
    }

    void
    shuffle()
    {
        for(std::size_t i = 0;
            i < cards.size(); ++i)
        {
            cards[i] = 1 + (i % 52);
        }
        for(std::size_t i = 0;
            i < cards.size() - 1; ++i)
        {
            auto const j = i +
                (rand() % (cards.size() - i));
            std::swap(
                cards[i], cards[j]);
        }
        pos = cards.begin();
    }

    char
    deal()
    {
        if(pos == cards.end())
            shuffle();
        return *pos++;
    }
};

//------------------------------------------------------------------------------

class game
{
    shoe shoe_;

public:
    explicit
    game(int decks)
        : shoe_(decks)
    {
    }
};

//------------------------------------------------------------------------------

class blackjack_table : public channel
{
    struct seat
    {
        enum state_t
        {
            empty,
            waiting,
            playing
        };

        user* u = nullptr;
        state_t state = empty;
    };

    server& srv_;
    timer_type timer_;
    game g_;

public:
    explicit
    blackjack_table(server& srv)
        :  channel(
            "Blackjack",
            srv.channel_list())
        , srv_(srv)
        , timer_(srv.make_executor())
        , g_(1)
    {
    }

protected:
    void
    on_insert(user&) override
    {
    }

    void
    on_erase(user&) override
    {
    }

    void
    on_dispatch(
        json::value& result,
        rpc_request& req,
        user& u) override
    {
        boost::ignore_unused(result, req, u);
        {
            throw rpc_except{rpc_error::method_not_found};
        }
    }

    void
    do_identify(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req, u);
    }
};

//------------------------------------------------------------------------------

class blackjack_service
    : public service
{
    server& srv_;

public:
    blackjack_service(
        server& srv)
        : srv_(srv)
    {
    }

    //--------------------------------------------------------------------------
    //
    // service
    //
    //--------------------------------------------------------------------------

    void
    on_start() override
    {
    }

    void
    on_stop() override
    {
    }
};

} // (anon)

//------------------------------------------------------------------------------

void
make_blackjack_service(
    server& srv)
{
    srv.insert(boost::make_unique<blackjack_service>(srv));
}
