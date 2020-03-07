//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_BLACKJACK_GAME_HPP
#define LOUNGE_BLACKJACK_GAME_HPP

#include "core/config.hpp"
#include <boost/beast/core/static_string.hpp>
#include <vector>

namespace blackjack {

//----------------------------------------------------------

class shoe
{
    std::string cards_;
    std::string::iterator pos_;

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

//----------------------------------------------------------

struct hand
{
    // cards[0]==0 for hole card
    beast::static_string<22> cards;
    int wager = 0;
    bool busted = false;
    bool twenty_one = false;
    bool blackjack = false;

    void
    clear()
    {
        cards.clear();
        wager = 0;
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
            auto const v =
                value(c);
            if(v != 1)
            {
                total += v;
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
    deal(shoe& s)
    {
        cards.push_back(s.deal());
        eval();
    }

    bool
    is_done() const
    {
        return busted || twenty_one;
    }
};

//----------------------------------------------------------

struct config
{
    int seats;
    int decks;
};

/** An instance of a blackjack game.
*/
template<class Callback>
class game
{
    Callback cb_;
    config cfg_;

public:
    shoe sh_;

    game(
        config const& cfg,
        Callback cb)
        : cfg_(cfg)
        , cb_(cb)
        , sh_(cfg_.decks)
    {
    }
};

//----------------------------------------------------------

} // blackjack

#endif
