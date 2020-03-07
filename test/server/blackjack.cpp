//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "blackjack/game.hpp"

#include "test_suite.hpp"

#include <iostream>

namespace blackjack {

beast::string_view
card_string(char c)
{
    return { "[]"
        "AC2C3C4C5C6C7C8C9CTCJCQCKC"
        "AH2H3H4H5H6H7H8H9HTHJHQHKH"
        "AS2S3S4S5S6S7S8S9STSJSQSKS"
        "AD2D3D4D5D6D7D8D9DTDJDQDKD" + 2 * c, 2 };
}

std::ostream&
operator<<(
    std::ostream& os,
    hand const& h)
{
    if(! h.cards.empty())
    {
        os << card_string(
            h.cards.front());
        for(auto it = std::next(
                h.cards.begin());
            it != h.cards.end(); ++it)
            os << "," << card_string(*it);
        if(h.busted)
            os << " bust";
        if(h.twenty_one)
            os << " twenty-one";
        if(h.blackjack)
            os << " blackjack";
    }
    return os;
}

class blackjack_test
{
public:
    test_suite::log_type log;
    
    blackjack_test()
        : log(test_suite::log())
    {
    }

    struct callback
    {
    };

    void
    testHand()
    {
        config cfg;
        cfg.decks = 1;
        cfg.seats = 1;
        game<callback> g(cfg, callback{});
        for(auto i = 0; i < 100; ++i)
        {
            hand h;
            while(! h.is_done())
                h.deal(g.sh_);
            log << h << "\n";
        }
    }

    void
    run()
    {
        testHand();
    }
};

TEST_SUITE(blackjack_test, "lounge.server.blackjack");

} // blackjack

