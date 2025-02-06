#ifndef BOT_UTILS
#define BOT_UTILS

#include <vector>
#include <map>
#include <tuple>
#include <future>
#include "deck.hpp"


// {Player Points, Dealer Card, Has Ace?, Can Split?, Can Double Down?}
using game_state = std::tuple<int, int, bool, bool, bool>;

enum bot_move {
    HIT, STAND, SPLIT, DOUBLE_DOWN 
};

class bot_model
{
private:
    std::map<game_state, bot_move> strategy;

public:
    bot_model() {}

    bot_model(const std::map<game_state, bot_move>& strategy){
        this->strategy = strategy;
    }

    ~bot_model() {}

    bot_move decide_bot_move(const game_state& state) {
        auto it = strategy.find(state);
        if (it != strategy.end()) {
            return it->second;
        }
        else{
            std::cerr << "Error: State not found: (" 
                        << std::get<0>(state) << ", " 
                        << std::get<1>(state) << ", " 
                        << std::get<2>(state) << ", " 
                        << std::get<3>(state) << ", " 
                        << std::get<4>(state) << ")" << std::endl;
        }
        
        // Default HIT, if no suitable bot_move found in strategy
        return HIT;
    }

};


class training_handler
{
private:
    int number_of_players;
    int current_player_index;
    int split_hand_offset;

    bool game_finished;

    // Dealer's hand on last index;
    std::vector<hand> hands;

    bot_model model;

    deck game_deck;

    void deal_cards() {
        card pulled;

        // Add two card to each player's hand, dealer's hand is last
        for (int i = 0; i < 2; i++)
        {
            for (hand& hand : hands) {
                pulled = game_deck.pull_card();

                hand.add_card(pulled);
            }
        }
    }

public:
    training_handler() {}

    training_handler(bot_model bot, const int players){
        number_of_players = players;
        current_player_index = 0;
        split_hand_offset = 0;

        model = bot;

        game_deck.prepare_deck();
    }

    ~training_handler() {}

    void prepare_game(){
        hands.clear();

        for(int i = 0; i < number_of_players - 1; i++){
            hands.emplace_back(PLAYER);
        }
        hands.emplace_back(DEALER);

        game_deck.prepare_deck();
        deal_cards();

        game_finished = false;
        current_player_index = 0;
        split_hand_offset = 0;
    }

    void set_finished(const bool is_finished){
        this->game_finished = is_finished;
    }

    hand get_dealer(){        
        return hands.back();
    }

    hand get_current_hand(){
        return hands[current_player_index + split_hand_offset];
    }

    int get_current_points(){
        return hands[current_player_index + split_hand_offset].get_points();
    }

    void pass_turn(){
        if(hands[current_player_index + split_hand_offset + 1].get_is_split()){
            split_hand_offset += 1;
        }
        else {
            current_player_index += 1;
        }

        if(current_player_index + split_hand_offset + 2 > hands.size()){
            draw_for_dealer();
        }
    }

    card pull_for_current(){
        card pulled = game_deck.pull_card();
        int new_value;

        hands[current_player_index + split_hand_offset].add_card(pulled);

        return pulled;
    }

    void draw_for_dealer(){
        card pulled;

        hands.back().update_for_dealer();

        // Dealer stands on soft 17
        // Game variant
        while(hands.back().get_points() <= 16){
            pulled = game_deck.pull_card();

            hands.back().add_card(pulled);
        }

        this->game_finished = true;
    }

    void split_current_hand(){
        hand split_hand = hands[current_player_index + split_hand_offset].get_split();
        
        if (current_player_index + split_hand_offset >= 0 && current_player_index + split_hand_offset < hands.size()) {
            hands.insert(hands.begin() + current_player_index + split_hand_offset + 1, split_hand);
        }
    }

    bool get_game_finished(){
        return this->game_finished;
    }

    int check_win_condition(){
        int total_return = 0;
        int factor = 1;

        for (size_t i = 0; i < hands.size() - 1; ++i) {
            if(hands[i].get_is_doubled()){
                factor = 2;
            }
            else{
                factor = 1;
            }

            if(hands.back().get_points() > 21) {
                if(hands[i].get_points() <= 21) {
                    total_return += 1 * factor;
                } else {
                    total_return -= 1 * factor;
                }
            } else {
                if(hands[i].get_points() > hands.back().get_points() && hands[i].get_points() <= 21) {
                    total_return += 1 * factor;
                }
                else if(hands[i].get_points() == hands.back().get_points() && hands[i].get_points() <= 21) {
                    total_return += 0 * factor;
                } 
                else if(hands[i].check_for_blackjack()){
                    total_return += 0.5;
                }
                else {
                    total_return -= 1 * factor;
                }
            }

        }

        // We won't punish for loosing, just not account for any wins
        return std::max(0, total_return);
    }

    int run_game(){
        int rounds_passed = 0;
        bool game_finished = false;
        bool splittable = false;

        game_state state;
        bot_move move;

        card pulled;

        prepare_game();

        while (true)
        {
            game_finished = get_game_finished();
            splittable = get_current_hand().check_for_split() && rounds_passed == 0;
            
            if(game_finished){
                int result = check_win_condition();
                return result;
            }

            if(get_current_hand().check_for_blackjack() && rounds_passed == 0){
                pass_turn();

                rounds_passed = 0;

                continue;
            }

            if(get_current_hand().get_is_split() && rounds_passed == 0){
                rounds_passed += 1;
            }

            state = {get_current_hand().get_points(), get_dealer().get_points(), get_current_hand().contains_ace(), splittable, rounds_passed == 0};
            move = model.decide_bot_move(state);

            if(move == HIT){
                pulled = pull_for_current();

                rounds_passed += 1;

                if(get_current_points() >= 21){
                    pass_turn();

                    rounds_passed = 0;
                }
            }
            else if(move == STAND){
                pass_turn();

                rounds_passed = 0;
            }
            else if(move == DOUBLE_DOWN){
                pulled = pull_for_current();
                get_current_hand().double_bet();

                pass_turn();

                rounds_passed = 0;
            }
            else if(move == SPLIT){
                split_current_hand();

                rounds_passed += 1;
            }

        }
    }
};

void mutate(std::map<game_state, bot_move>& strategy, float mutation_rate) {
    bot_move new_move;
    bool can_split, can_double_down;
    std::vector<bot_move> valid_moves;

    for (auto& [state, move] : strategy) {
        if ((rand() / static_cast<float>(RAND_MAX)) < mutation_rate) {
            valid_moves = {HIT, STAND};

            can_split = std::get<3>(state);
            can_double_down = std::get<4>(state);

            if (can_split) valid_moves.push_back(SPLIT);
            if (can_double_down) valid_moves.push_back(DOUBLE_DOWN);

            move = valid_moves[rand() % valid_moves.size()];
        }
    }
}

float evaluate_strategy(const std::map<game_state, bot_move>& strategy, int rounds) {
    bot_model bot(strategy);
    int wins = 0, result = 0;

    training_handler handler(bot, 2);
    
    for (int i = 0; i < rounds; ++i) {
        result = handler.run_game();

        wins += result;
    }

    return static_cast<float>(wins) / rounds;
}

std::vector<float> evaluate_population_async(const std::vector<std::map<game_state, bot_move>>& population, int rounds) {
    std::vector<std::future<float>> futures;

    for (const auto& strategy : population) {
        futures.push_back(std::async(std::launch::async, evaluate_strategy, strategy, rounds));
    }

    std::vector<float> fitness_scores;
    for (auto& future : futures) {
        fitness_scores.push_back(future.get());
    }

    return fitness_scores;
}

#endif