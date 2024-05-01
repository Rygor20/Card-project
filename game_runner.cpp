#include <iostream>

#include "deck.hpp"

char handle_input(const std::vector<char>&);

int main(int argc, char const *argv[])
{
    system("clear");

    deck card_deck;
    card_deck.prepare_deck();

    std::cout << "Starting deck size is: " << card_deck.get_starting() << std::endl;

    std::vector<card> pulled_cards;

    // for(int i = 0; i < 5; i++){
    //     card pulled = card_deck.pull_card();
    //     pulled_cards.push_back(pulled);

    //     std::cout << "Pulled card is: " << pulled << std::endl;

    //     std::string image = card_to_string(pulled.get_suit(), pulled.get_rank());
    //     std::cout << image << std::endl;

    //     std::cout << card_deck.get_size() << " cards remaining in deck" << std::endl;
    // }

    // std::string hand = show_hand(pulled_cards);
    // std::cout << hand << std::endl;

    char processed_input;
    bool missinput = false;
    std::vector<char> validInputs = {'1', '2'};

    card pulled;

    while (true)
    {
        system("clear");

        std::string hand = show_dealer_hand(pulled_cards);
        std::cout << hand << std::endl;

        if(missinput){
            std::cout << "Incorrect input! Try again!" << std::endl;
            missinput = false;
        }

        std::cout << "[1] Draw a card   [2] Leave" << std::endl;

        processed_input = handle_input(validInputs);

        if(processed_input == '1'){
            pulled = card_deck.pull_card();
            pulled_cards.push_back(pulled);
        }
        else if(processed_input == '2'){
            std::cout << "Exiting the program" << std::endl;
            break;
        }
        else if(processed_input == 'd'){
            break;
        }
        else{
            missinput = true;
        }
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