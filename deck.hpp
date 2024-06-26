#ifndef DECK_HEADER
#define DECK_HEADER

#include <iostream>
#include <vector>
#include <algorithm> 
#include <random> 
#include <string>

std::random_device rd; 
std::mt19937 twister(rd()); 

const std::string spade = "\u2660"; //5
const std::string heart = "\u2665";
const std::string diamond = "\u2666";
const std::string club = "\u2663";

const std::string cornerUL = "\U0000250F"; //9
const std::string cornerUR = "\U00002513";
const std::string cornerDL = "\U00002517";
const std::string cornerDR = "\U0000251B";

const std::string flatV = "\U00002503"; //9
const std::string flatH = "\U00002501";

enum card_suit {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

enum card_colour {
    BLACK, RED
};

enum card_rank {
    TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
};

enum who {
    DEALER, PLAYER
};

int value_from_rank(card_rank rank){
    if (rank >= TWO && rank <= TEN) {
        return rank + 2;
    } else if (rank >= JACK && rank <= KING) {
        return 10;
    } else if (rank == ACE) {
        return 11;
    } else {
        return -1;
    }
}

class card
{
private:
    card_suit suit;
    card_colour colour;
    card_rank rank;
    int value;
    
public:
    card() {}

    card(const card_rank rank, const card_suit suit){
        this->rank = rank;
        this->suit = suit;
        this->value = value_from_rank(rank);

        if(suit == CLUBS || suit == SPADES){
            this->colour = BLACK;
        }
        else{
            this->colour = RED;
        }
    }

    ~card() {}

    card_suit get_suit(){
        return this->suit;
    }

    card_colour get_colour(){
        return this->colour;
    }

    card_rank get_rank(){
        return this->rank;
    }

    int get_value(){
        return this->value;
    }

    bool compare(const card& other){
        return this->rank == other.rank;
    }

    card& operator=(const card& other) {
        if (this != &other) {
            this->suit = other.suit;
            this->colour = other.colour;
            this->rank = other.rank;
            this->value = other.value;
        }
        return *this;
    }

    bool operator==(const card& other) {
        return this->suit == other.suit &&
               this->get_colour() == other.colour &&
               this->get_rank() == other.rank &&
               this->get_value() == other.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const card& c) {
        os << "Rank: ";
        switch (c.rank) {
            case TWO: os << "Two"; break;
            case THREE: os << "Three"; break;
            case FOUR: os << "Four"; break;
            case FIVE: os << "Five"; break;
            case SIX: os << "Six"; break;
            case SEVEN: os << "Seven"; break;
            case EIGHT: os << "Eight"; break;
            case NINE: os << "Nine"; break;
            case TEN: os << "Ten"; break;
            case JACK: os << "Jack"; break;
            case QUEEN: os << "Queen"; break;
            case KING: os << "King"; break;
            case ACE: os << "Ace"; break;
            default: os << "Unknown"; break;
        }
        os << ", Suit: ";
        switch (c.suit) {
            case CLUBS: os << "Clubs"; break;
            case SPADES: os << "Spades"; break;
            case HEARTS: os << "Hearts"; break;
            case DIAMONDS: os << "Diamonds"; break;
            default: os << "Unknown"; break;
        }
        os << ", Color: ";
        switch (c.colour) {
            case BLACK: os << "Black"; break;
            case RED: os << "Red"; break;
            default: os << "Unknown"; break;
        }
        os << ", Value: " << c.value;
        return os;
    }
};


class deck
{
private:
    int starting_size;
    int current_size;

    std::vector<card> cards;

public:
    deck() {}
    ~deck() {}

    int get_starting(){
        return this->starting_size;
    }

    int get_size(){
        return this->current_size;
    }

    void prepare_deck(){
        std::vector<card> deck;

        for (int suit = CLUBS; suit <= DIAMONDS; ++suit) {
            for (int rank = TWO; rank <= ACE; ++rank) {
                deck.push_back(card(static_cast<card_rank>(rank), static_cast<card_suit>(suit)));
            }

            shuffle(deck.begin(), deck.end(), twister); 

            this->starting_size = deck.size();
            this->current_size = this->starting_size;

            this->cards = deck;
        }
    }

    card pull_card(){
        card pulled_card = cards[0];

        cards.erase(cards.begin());
        current_size -= 1;

        return pulled_card;
    }
};

std::string card_to_string(card_suit suit, card_rank rank){
    std::string result = "";

    result += cornerUL;
    for(int i = 0; i < 3; i++){
        result += flatH;
    }
    result += cornerUR;
    result += "\n";

    result += flatV;
    switch(suit){
        case SPADES: result += spade; break;
        case HEARTS: result += heart; break;
        case DIAMONDS: result += diamond; break;
        case CLUBS: result += club; break;
    }
    result += "  ";
    result += flatV;
    result += "\n";

    result += flatV;
    switch(rank){
        case TWO: result += "  2"; break;
        case THREE: result += "  3"; break;
        case FOUR: result += "  4"; break;
        case FIVE: result += "  5"; break;
        case SIX: result += "  6"; break;
        case SEVEN: result += "  7"; break;
        case EIGHT: result += "  8"; break;
        case NINE: result += "  9"; break;
        case TEN: result += " 10"; break;
        case JACK: result += "  J"; break;
        case QUEEN: result += "  Q"; break;
        case KING: result += "  K"; break;
        case ACE: result += "  A"; break;
    }
    result += flatV;
    result += "\n";

    result += cornerDL;
    for(int i = 0; i < 3; i++){
        result += flatH;
    }
    result += cornerDR;

    return result;
}

std::string show_hand(const std::vector<card> hand, const who person){
    std::string whole;

    for(int row = 0; row <= 3; row++){
        for(card card_in_hand: hand){
            std::string image = card_to_string(card_in_hand.get_suit(), card_in_hand.get_rank());
            std::string line;

            if(row == 0){
                line = image.substr(0, 15);
            }
            else if(row == 1){
                // line = image.substr(16, 11);
                if(person == DEALER && hand.size() == 2 && card_in_hand == hand[1]){
                    line = flatV + "?  " + flatV;
                }
                else{
                    line = image.substr(16, 11);
                }
            }
            else if(row == 2){
                // line = image.substr(28, 9);
                if(person == DEALER && hand.size() == 2 && card_in_hand == hand[1]){
                    line = flatV + "  ?" + flatV;
                }
                else{
                    line = image.substr(28, 9);
                }
            }
            else if(row == 3){
                line = image.substr(38, 15);
            }

            whole += line;
            whole += "   ";
        }
        whole += "\n";
    }

    return whole;
}

std::pair<int, std::string> calc_hand_value(const std::vector<card> hand, const who person){
    std::pair<int, std::string> result;
    int total = 0;
    int ace_count = 0;
    bool contains_ace = false;

    if(person == DEALER && hand.size() == 2){
        card first = hand[0];

        result.first = first.get_value();

        if(first.get_rank() == ACE){
            result.second = "11 (1) ?";
        }
        else{
            result.second = std::to_string(result.first) + " ?";
        }
    }
    else {
        for(card card_in_hand: hand){
            total += card_in_hand.get_value();

            if(card_in_hand.get_rank() == ACE){
                ace_count += 1;
                contains_ace = true;
            }
        }

        if(total > 21 && ace_count != 0){
            while(total > 21){
                total -= 10;
                ace_count -= 1;

                if(ace_count == 0){
                    break;
                }
            }
        }

        result.first = total;

        while(ace_count != 0){
            total -= 10;
            ace_count -= 1;
        }

        if(contains_ace){
            result.second = std::to_string(result.first) + " (" + std::to_string(total) + ")";
        }
        else{
            result.second = std::to_string(result.first);
        }
    }

    return result;
}

bool is_ace_hand_devalued(const std::string& input) {
    // Find the position of the '(' and ')'
    size_t openParenIndex = input.find('(');
    size_t closeParenIndex = input.find(')');

    // If '(' or ')' is not found, or if they are not in the expected format
    if (openParenIndex == std::string::npos || closeParenIndex == std::string::npos || openParenIndex >= closeParenIndex) {
        return false;
    }

    // Extract X and Y substrings
    std::string xStr = input.substr(0, openParenIndex);
    std::string yStr = input.substr(openParenIndex + 1, closeParenIndex - openParenIndex - 1);

    // Convert X and Y to integers
    int x_num = stoi(xStr);
    int y_num = stoi(yStr);

    // Compare X and Y
    return x_num == y_num;
}

#endif