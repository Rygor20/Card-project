#ifndef HANDLER_HEADER
#define HANDLER_HEADER

#include <iostream>
#include <map>

#include "deck.hpp"

// using SplitHands = std::map<int, std::vector<card>>;

class game_handler
{
private:
    int number_of_players;

    int current_player_index;

    bool game_finished;

    // Na ostatnim indeksie zawsze dealer;
    std::vector<int> player_points;

    std::vector<std::vector<card>> player_hands;

    std::map<int, std::vector<card>> split_hands;

    deck game_deck;

    void deal_cards(std::vector<std::vector<card>>& hands, deck& card_deck) {
        card pulled;

        // Add two card to each player's hand, dealer's hand is last
        for (int i = 0; i < 2; i++)
        {
            for (std::vector<card>& hand : hands) {
                pulled = card_deck.pull_card();

                hand.push_back(pulled);
            }
        }
    }

public:
    game_handler(const int players){
        this->number_of_players = players;
        this->current_player_index = 0;

        game_deck.prepare_deck();
    }

    ~game_handler() {}

    void prepare_game(){
        std::vector<std::vector<card>> hands(number_of_players);
        player_points.clear();
        split_hands.clear();

        game_deck.prepare_deck();

        deal_cards(hands, game_deck);

        add_players(hands);

        this->player_hands = hands;
        this->game_finished = false;
        this->current_player_index = 0;
    }

    void add_players(std::vector<std::vector<card>>& hands){
        this->player_hands = hands;

        int hand_value;

        for(std::vector<card>& hand : hands) {
            hand_value = calc_hand_value(hand, PLAYER).first;
            player_points.push_back(hand_value);
        }
    }

    void set_finished(const bool is_finished){
        this->game_finished = is_finished;
    }

    std::vector<card> get_dealer(){
        return player_hands.back();
    }

    std::vector<card> get_hand_at_index(const int index){
        return player_hands[index];
    }

    std::vector<card> get_current_hand(){
        return player_hands[current_player_index];
    }

    int get_points_at_index(const int index){
        return player_points[index];
    }

    int get_current_point(){
        return player_points[current_player_index];
    }

    void pass_turn(){
        current_player_index += 1;

        if(current_player_index + 1 > number_of_players){
            draw_for_dealer();
        }
    }

    card pull_for_player(const int index){
        card pulled = game_deck.pull_card();
        int new_value;

        player_hands[index].push_back(pulled);

        new_value = calc_hand_value(player_hands[index], PLAYER).first;
        player_points[index] = new_value;

        return pulled;
    }

    void draw_for_dealer(){
        card pulled;
        int new_value;

        while(player_points.back() <= 16){
            pulled = game_deck.pull_card();

            player_hands.back().push_back(pulled);

            new_value = calc_hand_value(player_hands.back(), DEALER).first;
            player_points.back() = new_value;
        }

        this->game_finished = true;
    }

    bool check_for_split(std::vector<card> hand){
        // if(hand.size() == 2 && hand[0] == hand[1]){
        //     return true;
        // }
        if(hand.size() == 2 && hand[0].compare(hand[1])){
            return true;
        }

        return false;
    }

    void split_hand_at_index(const int index){
        card popped = player_hands[index][1];
        std::vector<card> second_hand;
        second_hand.push_back(popped);

        player_hands[index].pop_back();

        split_hands.insert({index, second_hand});
    }

    void show_game_state(){
        std::string string_hand;
        std::vector<card> card_hand = get_dealer();

        if(!game_finished){
            std::cout << "Dealer: " << calc_hand_value(card_hand, DEALER).second << std::endl;
            string_hand = show_hand(card_hand, DEALER);
        }
        else{
            std::cout << "Dealer: " << calc_hand_value(card_hand, PLAYER).second << std::endl;
            string_hand = show_hand(card_hand, PLAYER);
        }

        std::cout << string_hand << std::endl;

        for(int i = 0; i < player_hands.size() - 1; i++){
            card_hand = get_hand_at_index(i);

            std::cout << "Player " << i + 1 << ": " << calc_hand_value(card_hand, PLAYER).second << std::endl;
            string_hand = show_hand(card_hand, PLAYER);

            // if(split_hands.find(i) != split_hands.end()){
            //     card_hand = split_hands[i];

            //     std::cout << "Player " << i + 1 << " split:" << calc_hand_value(card_hand, PLAYER).second << std::endl;
            //     string_hand = show_hand(card_hand, PLAYER);
            // }

            std::cout << string_hand << std::endl;

            if(split_hands.find(i) != split_hands.end()){
                card_hand = split_hands[i];

                std::cout << "Player " << i + 1 << " split:" << calc_hand_value(card_hand, PLAYER).second << std::endl;
                string_hand = show_hand(card_hand, PLAYER);

                std::cout << string_hand << std::endl;
            }
        }
    }

    bool get_game_finished(){
        return this->game_finished;
    }

    void check_win_condition(){
        if(player_points.back() > 21){
            std::cout << "Dealer busted, everybody wins!" << std::endl;

            return;
        }

        for (size_t i = 0; i < player_points.size() - 1; ++i) {
            if(player_points[i] > 21){
                std::cout << "Player " << i + 1 << " busts! Better luck next time!" << std::endl;
            }
            else if(player_points[i] > player_points.back() || player_points[i] == 21){
                std::cout << "Player " << i + 1 << " wins! Congratulations!" << std::endl;
            }
            else{
                std::cout << "Player " << i + 1 << " loses! Better luck next time!" << std::endl;
            }
        }
    }

};

#endif