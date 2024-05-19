#include <iostream>

#include "deck.hpp"
#include "game_handler.hpp"
#include "bot_player.hpp"

char handle_input(const std::vector<char>&);

void deal_cards(std::vector<std::vector<card>>&, deck&);

int main(int argc, char const *argv[])
{
    system("clear");

    char processed_input;
    bool game_finished = false;
    bool missinput = false;
    bool splittable = false;

    std::vector<char> valid_inputs = {'1', '2', '3'};

    card pulled;
    card upcard;

    std::string hand;

    game_handler handler = game_handler(2);
    handler.prepare_game();

    bot_player bot = bot_player(handler.get_hand_at_index(0));

    upcard = handler.get_dealer()[0];

    while (true)
    {
        game_finished = handler.get_game_finished();

        handler.show_game_state();

        // if(!game_finished){
        //     std::cout << "Dealer: " << calc_hand_value(handler.get_dealer(), DEALER).second << std::endl;
        //     hand = show_hand(handler.get_dealer(), DEALER);
        // }
        // else{
        //     std::cout << "Dealer: " << calc_hand_value(handler.get_dealer(), PLAYER).second << std::endl;
        //     hand = show_hand(handler.get_dealer(), PLAYER);
        // }
        // std::cout << hand << std::endl;

        // std::cout << "\n" << std::endl;

        // std::cout << "Player: " << calc_hand_value(handler.get_hand_at_index(0), PLAYER).second << std::endl;
        // hand = show_hand(handler.get_hand_at_index(0), PLAYER);
        // std::cout << hand << std::endl;

        if(game_finished){
            handler.check_win_condition();
        }

        if(handler.check_for_split(handler.get_hand_at_index(0))){
            std::cout << "Hand is splittable" << std::endl;
        }
        else{
            std::cout << "Hand is not splittable" << std::endl;
        }

        if(missinput){
            std::cout << "Incorrect input! Try again!" << std::endl;
            missinput = false;
        }

        if(!game_finished){
            std::cout << "[1] Hit  [2] Stand  [3] Leave" << std::endl;

            std::cout << "Should you hit? : " << bot.should_hit(upcard.get_value()) << std::endl;
        }
        else{
            std::cout << "[1] Play again  [2] Leave" << std::endl;
        }

        if(!game_finished){
            processed_input = handle_input({'1', '2', '3'});
        }
        else{
            processed_input = handle_input({'1', '2'});
        }
        //processed_input = handle_input(valid_inputs);

        if(processed_input == '1' && !game_finished){
            pulled = handler.pull_for_player(0);

            bot.add_card(pulled);

            if(handler.get_points_at_index(0) >= 21){
                handler.set_finished(true);
            }        //processed_input = handle_input(valid_inputs);
        }
        else if(processed_input == '1' && game_finished){
            handler.prepare_game();

            bot.set_hand(handler.get_hand_at_index(0));

            upcard = handler.get_dealer()[0];
        }
        else if(processed_input == '2' && !game_finished){
            handler.draw_for_dealer();
        }
        else if((processed_input == '3' && !game_finished) || (processed_input == '2' && game_finished)){
            std::cout << "Exiting the program" << std::endl;
            break;
        }
        else if(processed_input == 'd'){
            break;
        }
        else{
            missinput = true;
        }

        system("clear");
    }

    return 0;
}

char handle_input(const std::vector<char>& valid_inputs) {
    std::string input;
    std::cin >> input;

    if (std::cin.eof()) {
        return 'd';
    } else if (input.length() != 1) {
        return 'e';
    }

    char user_input = input[0];
    for (char valid_input : valid_inputs) {
        if (user_input == valid_input) {
            return user_input;
        }
    }

    return 'e';
}