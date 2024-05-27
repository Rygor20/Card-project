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

    std::vector<char> valid_inputs;

    card pulled;
    card upcard;

    std::string hand;

    game_handler handler = game_handler(3);
    handler.prepare_game();

    bot_player bot = bot_player(handler.get_hand_at_index(0));

    upcard = handler.get_dealer()[0];

    while (true)
    {
        game_finished = handler.get_game_finished();
        splittable = handler.check_for_split(handler.get_hand_at_index(0));

        handler.show_game_state();

        if(game_finished){
            handler.check_win_condition();
        }

        if(missinput){
            std::cout << "Incorrect input! Try again!" << std::endl;
            missinput = false;
        }

        if(splittable){
            std::cout << "You can split your hand" << std::endl;
        }

        if(!game_finished && splittable){
            std::cout << "[1] Split [2] Don't split" << std::endl;

            valid_inputs = {'1', '2'};
        }
        else if(!game_finished){
            std::cout << "[1] Hit  [2] Stand  [3] Leave" << std::endl;

            std::cout << "Should you hit? : " << bot.should_hit(upcard.get_value()) << std::endl;

            valid_inputs = {'1', '2', '3'};
        }
        else{
            std::cout << "[1] Play again  [2] Leave" << std::endl;

            valid_inputs = {'1', '2'};
        }

        processed_input = handle_input(valid_inputs);

        //!!!!!!!!!!!!
        // REFORMAT CODE FOR CURRENT PLAYER AND NOT HARD CODING FOR INDEX '0'
        //!!!!!!!!!!!!

        if(processed_input == '1' && !game_finished && splittable){
            handler.split_hand_at_index(0);
        }
        else if(processed_input == '1' && !game_finished){
            pulled = handler.pull_for_player(0);

            bot.add_card(pulled);

            if(handler.get_points_at_index(0) >= 21){
                handler.set_finished(true);
            }
        }
        else if(processed_input == '1' && game_finished){
            handler.prepare_game();

            bot.set_hand(handler.get_hand_at_index(0));

            upcard = handler.get_dealer()[0];
        }
        else if(processed_input == '2' && !game_finished){
            handler.pass_turn();
            //handler.draw_for_dealer();
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