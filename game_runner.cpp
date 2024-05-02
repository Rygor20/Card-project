#include <iostream>

#include "deck.hpp"

char handle_input(const std::vector<char>&);

void deal_cards(std::vector<std::vector<card>>&, deck&);

int main(int argc, char const *argv[])
{
    system("clear");

    deck card_deck;
    card_deck.prepare_deck();

    std::cout << "Starting deck size is: " << card_deck.get_starting() << std::endl;

    //std::vector<card> pulled_cards;

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
    // std::vector<card> dealer_hand;
    // std::vector<card> player_hand;

    std::string hand;

    std::vector<std::vector<card>> hands(2);
    // hands.push_back(player_hand);
    // hands.push_back(dealer_hand);

    deal_cards(hands, card_deck);

    //std::cout << "Dealer's hand size: " << std::to_string(hands[1].size()) << std::endl;

    while (true)
    {
        std::cout << "Dealer: " << calc_hand_value(hands[1], DEALER).second << std::endl;
        // hand = show_dealer_hand(hands[1]);
        hand = show_hand(hands[1], DEALER);
        std::cout << hand << std::endl;

        std::cout << "\n" << std::endl;

        std::cout << "Player: " << calc_hand_value(hands[0], PLAYER).second << std::endl;
        // hand = show_hand(hands[0]);
        hand = show_hand(hands[0], PLAYER);
        std::cout << hand << std::endl;

        if(missinput){
            std::cout << "Incorrect input! Try again!" << std::endl;
            missinput = false;
        }

        std::cout << "[1] Draw a card   [2] Leave" << std::endl;

        processed_input = handle_input(validInputs);

        if(processed_input == '1'){
            pulled = card_deck.pull_card();
            hands[0].push_back(pulled);
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

void deal_cards(std::vector<std::vector<card>>& hands, deck& card_deck) {
    card pulled;

    // Add two card to each player's hand, dealer's hand is last
    for (int i = 0; i < 2; i++)
    {
        //std::cout << "Dealing first card" << std::endl;

        for (std::vector<card>& hand : hands) {
            pulled = card_deck.pull_card();

            //std::cout << "Pulled card is: " << pulled << std::endl;

            hand.push_back(pulled);
        }
    }

    //std::cout << "Dealer's hand size after dealing: " << hands[1].size() << std::endl;
}