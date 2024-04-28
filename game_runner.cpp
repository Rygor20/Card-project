#include<iostream>

#include "deck.hpp"

int main(int argc, char const *argv[])
{
    deck card_deck;
    card_deck.prepare_deck();

    std::cout << "Starting deck size is: " << card_deck.get_starting() << std::endl;

    for(int i = 0; i < 5; i++){
        card pulled = card_deck.pull_card();
        std::cout << "Pulled card is: " << pulled << std::endl;

        std::string image = card_to_string(pulled.get_suit(), pulled.get_rank());
        std::cout << image << std::endl;

        std::cout << card_deck.get_size() << " cards remaining in deck" << std::endl;
    }

    return 0;
}
