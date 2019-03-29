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
#include <boost/beast/core/static_string.hpp>
#include <boost/make_unique.hpp>
#include <vector>
#include <utility>

namespace {

//------------------------------------------------------------------------------

class shoe
{
    std::vector<char> cards_;
    std::vector<char>::iterator pos_;

public:
    explicit
    shoe(int decks)
    {
        cards_.resize(decks * 52);
        shuffle();
    }

    void
    shuffle()
    {
        for(std::size_t i = 0;
            i < cards_.size(); ++i)
        {
            cards_[i] = 1 + (i % 52);
        }
        for(std::size_t i = 0;
            i < cards_.size() - 1; ++i)
        {
            auto const j = i +
                (rand() % (cards_.size() - i));
            std::swap(
                cards_[i], cards_[j]);
        }
        pos_ = cards_.begin();
    }

    char
    deal()
    {
        if(pos_ == cards_.end())
            shuffle();
        return *pos_++;
    }
};

//------------------------------------------------------------------------------

struct hand
{
    beast::static_string<22> cards;
    bool busted = false;
    bool twenty_one = false;
    bool blackjack = false;

    void
    clear()
    {
        cards.clear();
        busted = false;
        twenty_one = false;
        blackjack = false;
    }

    // Returns the value of a card,
    // always returns 1 for aces
    static
    int
    value(char c)
    {
        auto v = 1 + ((c - 1) % 13);
        if( v > 10)
            v = 10;
        return v;
    }

    void
    eval()
    {
        int aces = 0;
        int total = 0;
        for(auto c : cards)
        {
            if(c != 1)
            {
                total += c;
            }
            else
            {
                ++aces;
                total += 11;
            }
        }
        while(total > 21 && aces--)
            total -= 10;

        busted = total > 21;
        twenty_one = total == 21;
        blackjack =
            twenty_one &&
            cards.size() == 2;
    }

    bool
    is_finished() const
    {
        return busted || twenty_one;
    }
};

//------------------------------------------------------------------------------

class game
{
    enum what
    {
        under,
        twentyone,
        natural,
        bust
    };

    shoe shoe_;

    // 0    = dealer
    // 1..5 = player
    std::vector<hand> v_;

public:
    explicit
    game(int decks)
        : shoe_(decks)
    {
        BOOST_ASSERT(
            decks >= 1 && decks <= 8);

        v_.resize(6);

        // dealer always seated
        //v_[0].seated = true;
    }

    static
    beast::string_view
    card_string(char c)
    {
        static char const cards[] =
            "AC2C3C4C5C6C7C8C9CTCJCQCKC"
            "AH2H3H4H5H6H7H8H9HTHJHQHKH"
            "AS2S3S4S5S6S7S8S9STSJSQSKS"
            "AD2D3D4D5D6D7D8D9DTDJDQDKD";
        BOOST_ASSERT(c >= 1 && c <= 52);
        return { cards + 2 * (c - 1), 2 };
    }

    void
    clear()
    {
        for(auto& e : v_)
            e.clear();
    }

    void
    start()
    {
    }

    void
    deal_one(hand& h)
    {
        auto const c = shoe_.deal();

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
        boost::ignore_unused(srv_);
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
