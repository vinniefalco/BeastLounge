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
#include <functional>
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

    void
    hit(shoe& s)
    {
        cards.push_back(s.deal());
    }

    bool
    is_finished() const
    {
        return busted || twenty_one;
    }

    void
    to_json(json::value& jv) const
    {
        auto& arr = jv.emplace_array();
        arr.insert(arr.end(),
            cards.begin(), cards.end());
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

    seat()
    {
        hands.resize(1);
    }

    void
    to_json(json::value& jv) const
    {
        jv = json::object{};
        jv.emplace("hands", hands);
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
    int turn_ = 1;

public:
    explicit
    game(int decks)
        : shoe_(decks)
    {
        BOOST_ASSERT(
            decks >= 1 && decks <= 6);
        seat_.resize(6);
        seat_[0].state = seat::dealer;
    }

    std::size_t
    find(user const& u) const
    {
        for(auto const& s : seat_)
            if( s.state != seat::open &&
                s.u == &u)
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

    // Join the game as a player.
    // 1..5 = assigned seat
    // 0 = no open seat
    // -1 = already joined
    int
    play(user& u)
    {
        if(find(u) != 0)
            return -1;
        for(auto& s : seat_)
        {
            if(s.state == seat::open)
            {
                s.state = seat::waiting;
                s.u = &u;
                return &s - &seat_.front();
            }
        }
        return 0;
    }

    // Leave the game as a player.
    //  1 = now leaving, was playing
    //  2 = now open, was waiting
    // -1 = not playing
    // -2 = already leaving
    int
    watch(user& u)
    {
        auto const i = find(u);
        if(! i)
            return -1;
        switch(seat_[i].state)
        {
        case seat::waiting:
            seat_[i].state = seat::open;
            return 2;

        case seat::playing:
            seat_[i].state = seat::leaving;
            return 1;

        case seat::leaving:
            return -2;

        default:
            break;
        }
        return -3;
    }

    // Surrender the hand and leave
    //  1 = success
    // -1 = not playing
    int
    surrender(user& u)
    {
        auto const i = find(u);
        if(! i)
            return -1;
        seat_[i].state = seat::open;
        return 1;
    }

    void
    hit()
    {
        seat_[1].hands[0].hit(shoe_);
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

private:
    // Post a call to the strand
    template<class... Args>
    void
    post(Args&&... args)
    {
        net::post(
            timer_.get_executor(),
            beast::bind_front_handler(
                std::forward<Args>(args)...));
    }

    //--------------------------------------------------------------------------
    //
    // channel
    //
    //--------------------------------------------------------------------------

    void
    on_insert(user& u) override
    {
        post(
            &table::do_insert,
            this,
            shared_from(&u));
    }

    void
    on_erase(user& u) override
    {
        post(
            &table::do_erase,
            this,
            std::reference_wrapper<user>(u));
    }

    void
    on_dispatch(rpc_call& rpc) override
    {
        if(rpc.method == "play")
        {
            post(&table::do_play, this, std::move(rpc));
        }
        else if(rpc.method == "watch")
        {
            post(&table::do_watch, this, std::move(rpc));
        }
        else if(rpc.method == "hit")
        {
            post(&table::do_hit, this, std::move(rpc));
        }
        else if(rpc.method == "stand")
        {
            post(&table::do_stand, this, std::move(rpc));
        }
        else
        {
            rpc.fail(rpc_code::method_not_found);
        }
    }

    //--------------------------------------------------------------------------
    //
    // table
    //
    //--------------------------------------------------------------------------

    void
    update(beast::string_view action)
    {
        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "update";
        jv["action"] = action;
        jv["game"] = g_;
        send(jv);
    }

    void
    do_insert(boost::shared_ptr<user> sp)
    {
        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "update";
        jv["action"] = "init";
        jv["game"] = g_;
        sp->send(jv);
    }

    void
    do_erase(user& u)
    {
        auto const result = g_.surrender(u);
        if(result == 1)
            update("surrender");
    }

    void
    do_play(rpc_call&& rpc)
    {
        try
        {
            // TODO Optional seat choice
            auto result = g_.play(*rpc.u);
            if(result == 0)
                rpc.fail("No open seat");
            if(result == -1)
                rpc.fail("Already playing");
            update("play");
            rpc.complete();
        }
        catch(rpc_error const& e)
        {
            rpc.complete(e);
        }
    }

    void
    do_watch(rpc_call&& rpc)
    {
        try
        {
            auto result = g_.watch(*rpc.u);
            if(result == -1)
                rpc.fail("Not playing");
            if(result == -2)
                rpc.fail("Already leaving");
            update("watch");
            rpc.complete();
        }
        catch(rpc_error const& e)
        {
            rpc.complete(e);
        }
    }

    void
    do_hit(rpc_call&& rpc)
    {
        try
        {
            g_.hit();
            update("hit");
            rpc.complete();
        }
        catch(rpc_error const& e)
        {
            rpc.complete(e);
        }
    }

    void
    do_stand(rpc_call&& rpc)
    {
        try
        {
        }
        catch(rpc_error const& e)
        {
            rpc.complete(e);
        }
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
