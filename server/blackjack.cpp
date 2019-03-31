//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "channel_list.hpp"
#include "rpc.hpp"
#include "server.hpp"
#include "service.hpp"
#include "types.hpp"
#include "user.hpp"
#include <boost/beast/_experimental/json/value.hpp>
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
    // cards[0]==0 for hole card
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

    void
    to_json(json::value& jv) const
    {
        jv.emplace_string().assign(
            cards.data(), cards.size());
    }
};

//------------------------------------------------------------------------------

// Represents one of the five seating
// positions of the blackjack playfield.
//
struct seat
{
    enum state_t
    {
        dealer,
        waiting,
        playing,
        leaving,
        open
    };

    user* u = nullptr;
    state_t state = open;
    std::vector<hand> hands;

    void
    to_json(json::value& jv) const
    {
        jv = json::object{};
        switch(state)
        {
        case dealer:
            jv["state"] = "dealer";
            break;
        case waiting:
            jv["state"] = "waiting";
            jv["user"] = u->name;
            break;
        case playing:
            jv["state"] = "playing";
            jv["user"] = u->name;
            break;
        case leaving:
            jv["state"] = "leaving";
            jv["user"] = u->name;
            break;
        case open:
            jv["state"] = "open";
            break;
        }
        jv.emplace("hands", hands);
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
    std::vector<seat> seat_;

    // which seat's turn
    int turn_ = 0;

public:
    explicit
    game(int decks)
        : shoe_(decks)
    {
        BOOST_ASSERT(
            decks >= 1 && decks <= 8);

        seat_.resize(6);

        seat_[0].state = seat::dealer;
    }

    void
    deal_one(hand& h)
    {
        auto const c = shoe_.deal();
    }

    std::size_t
    find(user const& u) const
    {
        for(auto const& s : seat_)
            if(s.u == &u)
                return &s - &seat_.front();
        return 0;
    }

    std::size_t
    insert(user& u)
    {
        for(auto& s : seat_)
            if(s.state == seat::open)
            {
                s.state = seat::waiting;
                s.u = &u;
                return &s - &seat_.front();
            }
        return 0;
    }

    void
    to_json(json::value& jv) const
    {
        auto& obj = jv.emplace_object();
        {
            auto& arr = obj.emplace(
                "seats", json::array{}).first->second;
            for(std::size_t i = 0;
                i < seat_.size(); ++i)
                arr.emplace_back(seat_[i]);
        }
    }
};

//------------------------------------------------------------------------------

class table : public channel
{
    using lock_guard =
        std::lock_guard<std::mutex>;
   
    server& srv_;
    timer_type timer_;
    std::mutex mutable mutex_;
    game g_;

public:
    explicit
    table(server& srv)
        :  channel(
            3,
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
    on_insert(user& u) override
    {
        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "update";
        jv["action"] = "init";
        {
            lock_guard lock(mutex_);
            jv["game"] = g_;
        }
        u.send(jv);
    }

    void
    on_erase(user&) override
    {
        // TODO surrender hand
    }

    void
    on_dispatch(
        json::value& result,
        rpc_request& req,
        user& u) override
    {
        boost::ignore_unused(result, u);
        if(req.method == "join")
        {
            do_join(result, req, u);
        }
        else if(req.method == "leave")
        {
            do_leave(result, req, u);
        }
        else if(req.method == "play")
        {
            do_play(result, req, u);
        }
        else if(req.method == "watch")
        {
            do_watch(result, req, u);
        }
        else if(req.method == "hit")
        {
            do_hit(result, req, u);
        }
        else if(req.method == "stand")
        {
            do_stand(result, req, u);
        }
        else
        {
            throw rpc_except{rpc_error::method_not_found};
        }
    }

    void
    checked_user(user& u)
    {
        if(u.name.empty())
            throw rpc_except(
                "No identity set");
    }

    void
    do_join(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);

        checked_user(u);

        if(! insert(u))
            throw rpc_except(
                "Already joined");
    }

    void
    do_leave(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);

        erase(u);
    }

    void
    do_play(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);
        // TODO Optional seat choice

        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "update";
        jv["action"] = "play";
        {
            lock_guard lock(mutex_);
            if(g_.find(u) != 0)
                throw rpc_except(
                    "Already playing");
            if(g_.insert(u) == 0)
                throw rpc_except(
                    "No open seats");
            jv["game"] = g_;
        }
        send(jv);
    }

    void
    do_watch(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);
    }

    void
    do_hit(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);
    }

    void
    do_stand(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);
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
        insert<table>(srv_.channel_list(), srv_);
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
