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
    bool missinput = false;
    std::vector<char> validInputs = {'1', '2', '3'};

    card pulled;
    card upcard;

    std::string hand;

    game_handler handler = game_handler(2);
    handler.prepare_game();

    bot_player bot = bot_player(handler.get_hand_at_index(0));

    upcard = handler.get_dealer()[0];

    while (true)
    {
        if(!handler.get_game_finished()){
            std::cout << "Dealer: " << calc_hand_value(handler.get_dealer(), DEALER).second << std::endl;
            hand = show_hand(handler.get_dealer(), DEALER);
        }
        else{
            std::cout << "Dealer: " << calc_hand_value(handler.get_dealer(), PLAYER).second << std::endl;
            hand = show_hand(handler.get_dealer(), PLAYER);
        }
        std::cout << hand << std::endl;

        std::cout << "\n" << std::endl;

        std::cout << "Player: " << calc_hand_value(handler.get_hand_at_index(0), PLAYER).second << std::endl;
        hand = show_hand(handler.get_hand_at_index(0), PLAYER);
        std::cout << hand << std::endl;

        if(handler.get_game_finished()){
            handler.check_win_condition();
        }

        if(missinput){
            std::cout << "Incorrect input! Try again!" << std::endl;
            missinput = false;
        }

        if(!handler.get_game_finished()){
            std::cout << "[1] Hit  [2] Stand  [3] Leave" << std::endl;

            std::cout << "Should you hit? : " << bot.should_hit(upcard.get_value()) << std::endl;
        }
        else{
            std::cout << "[1] Play again  [2] Leave" << std::endl;
        }

        processed_input = handle_input(validInputs);

        if(processed_input == '1' && !handler.get_game_finished()){
            pulled = handler.pull_for_player(0);

            bot.add_card(pulled);

            if(handler.get_points_at_index(0) >= 21){
                handler.set_finished(true);
            }
        }
        else if(processed_input == '1' && handler.get_game_finished()){
            handler.prepare_game();

            bot.set_hand(handler.get_hand_at_index(0));

            upcard = handler.get_dealer()[0];
        }
        else if(processed_input == '2' && !handler.get_game_finished()){
            handler.draw_for_dealer();
        }
        else if((processed_input == '3' && !handler.get_game_finished()) || (processed_input == '2' && handler.get_game_finished())){
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

char handle_input(const std::vector<char>& validInputs) {
    std::string input;
    std::cin >> input;

    if (std::cin.eof()) {
        return 'd';
    } else if (input.length() != 1) {
        return 'e';
    }

    char userInput = input[0];
    for (char validInput : validInputs) {
        if (userInput == validInput) {
            return userInput;
        }
    }

    return 'e';
}