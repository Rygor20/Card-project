#ifndef BOT_HEADER
#define BOT_HEADER

#include <iostream>
#include <vector>
#include <map>
#include "deck.hpp"

enum action { 
    HIT,   STAND 
};

struct strategy_cell {
    action bot_action;
};

// Define a type alias for the strategy chart
using strategy_chart = std::map<int, std::map<int, strategy_cell>>;

class bot_player
{
private:
    std::vector<card> hand;
    int hand_value;

    strategy_chart hard_totals_chart = {
        // Player's Hand -> Dealer's Upcard -> Action
        {5,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {6,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {7,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {8,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {9,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {10, {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {11, {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {12, {{2, {HIT}}, {3, {HIT}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {13, {{2, {STAND}}, {3, {STAND}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {14, {{2, {STAND}}, {3, {STAND}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {15, {{2, {STAND}}, {3, {STAND}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {16, {{2, {STAND}}, {3, {STAND}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}}
    };

    strategy_chart soft_totals_chart = {
        // Player's Hand -> Dealer's Upcard -> Action
        {2,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {3,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {4,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {5,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {6,  {{2, {HIT}}, {3, {HIT}}, {4, {HIT}}, {5, {HIT}}, {6, {HIT}}, {7, {HIT}}, {8, {HIT}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}},
        {7,  {{2, {STAND}}, {3, {STAND}}, {4, {STAND}}, {5, {STAND}}, {6, {STAND}}, {7, {STAND}}, {8, {STAND}}, {9, {HIT}}, {10, {HIT}}, {11, {HIT}}}}
    };

    bool contains_ace(){
        bool contains_ace = false;

        for(card card_in_hand: hand){
            if(card_in_hand.get_rank() == ACE){
                contains_ace = true;
            }
        }

        return contains_ace;
    }

public:
    bot_player() {}

    bot_player(std::vector<card> starting_hand){
        this->hand = starting_hand;
        this->hand_value = calc_hand_value(starting_hand, PLAYER).first;
    }

    ~bot_player() {}

    void set_hand(std::vector<card> starting_hand){
        this->hand = starting_hand;
        this->hand_value = calc_hand_value(starting_hand, PLAYER).first;
    }

    void add_card(card pulled){
        this->hand.push_back(pulled);
        this->hand_value = calc_hand_value(hand, PLAYER).first;
    }

    bool should_hit(const int upcard){
        bool decision = false;
        //std::cout << "Your " << hand_value << " vs Dealer's " << upcard << std::endl;

        if(contains_ace() && is_ace_hand_devalued(calc_hand_value(hand, PLAYER).second)){
            if((hand_value - 10) < 8){
                if(soft_totals_chart[hand_value][upcard].bot_action == HIT){
                    decision = true;
                }
            }
        }
        else{
            if(hand_value < 17){
                if(hard_totals_chart[hand_value][upcard].bot_action == HIT){
                    decision = true;
                }
            }
        }

        return decision;
    }
};

#endif