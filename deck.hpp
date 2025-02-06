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

    bool compare(const card& other) const {
        // Group TEN, JACK, QUEEN, KING into a "high rank" group
        bool isHighRank = (rank == TEN || rank == JACK || rank == QUEEN || rank == KING);
        bool otherIsHighRank = (other.rank == TEN || other.rank == JACK || other.rank == QUEEN || other.rank == KING);

        if (isHighRank && otherIsHighRank) {
            return true;
        }

        return rank == other.rank;
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


int calc_hand_value(const std::vector<card>, const who);
std::string card_to_string(card_suit, card_rank);


class hand
{
private:
    std::vector<card> cards;
    int points;
    bool is_player;
    bool is_split;
    bool is_doubled;

    void update_points(){
        int hand_value = calc_hand_value(cards, is_player ? PLAYER : DEALER);
        points = hand_value;
    }

public:
    hand() {}

    hand(const who person){
        points = 0;
        is_player = (person == PLAYER);
        is_split = false;
        is_doubled = false;
    }

    ~hand() {}

    void add_card(const card& card){
        cards.push_back(card);
        update_points();
    }

    card get_upcard(){
        if(!is_player){
            return cards[0];
        }
        else{
            throw std::logic_error("Can't define 'upcard' for non-dealer hand.");
        }
    }

    bool contains_ace(){
        bool contains_ace = false;

        for(card card_in_hand: cards){
            if(card_in_hand.get_rank() == ACE){
                contains_ace = true;
            }
        }

        return contains_ace;
    }

    bool check_for_blackjack(){
        if(cards.size() == 2 && points == 21 && !is_split){
            return true;
        }

        return false;
    }

    bool check_for_split(){
        if(cards.size() == 2 && cards[0].compare(cards[1]) && !is_split){
            return true;
        }

        return false;
    }

    void double_bet(){
        is_doubled = true;
    }

    hand get_split(){
        if(!check_for_split()) {
            throw std::logic_error("Hand cannot be split.");
        }

        hand new_hand(is_player ? PLAYER : DEALER);
        new_hand.add_card(cards[1]);
        new_hand.is_split = true;

        cards.pop_back();
        update_points();

        return new_hand;
    }

    int get_points(){
        return points;
    }

    bool get_is_player(){
        return is_player;
    }

    bool get_is_split(){
        return is_split;
    }

    bool get_is_doubled(){
        return is_doubled;
    }

    std::string show_hand(bool reveal_second_card){
        std::string whole;

        for(int row = 0; row <= 3; row++){
            for(card card_in_hand: cards){
                std::string image = card_to_string(card_in_hand.get_suit(), card_in_hand.get_rank());
                std::string line;

                if(row == 0){
                    line = image.substr(0, 15);
                }
                else if(row == 1){
                    if(!is_player && cards.size() == 2 && card_in_hand == cards[1] && !reveal_second_card){
                        line = flatV + "?  " + flatV;
                    }
                    else{
                        line = image.substr(16, 11);
                    }
                }
                else if(row == 2){
                    if(!is_player && cards.size() == 2 && card_in_hand == cards[1] && !reveal_second_card){
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

    std::string show_hand_content(bool show_full){
        std::string result;
        int total = 0;
        int total_temp = 0;
        int ace_count = 0;
        bool contains_ace = false;

        if(!show_full){
            card first = cards[0];

            if(first.get_rank() == ACE){
                result = "11 (1) ?";
            }
            else{
                result = std::to_string(first.get_value()) + " ?";
            }
        }
        else {
            for(card card_in_hand: cards){
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

            total_temp = total;

            while(ace_count != 0){
                total -= 10;
                ace_count -= 1;
            }

            if(contains_ace){
                result = std::to_string(total_temp) + " (" + std::to_string(total) + ")";
            }
            else{
                result = std::to_string(total_temp);
            }
        }

        return result;
    }

    void update_for_dealer(){
        int hand_value = calc_hand_value(cards, PLAYER);
        points = hand_value;
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

int calc_hand_value(const std::vector<card> cards, const who person){
    int result;
    int total = 0;
    int ace_count = 0;
    bool contains_ace = false;

    if(person == DEALER && cards.size() == 2){
        card first = cards[0];

        result = first.get_value();
    }
    else {
        for(card card_in_hand: cards){
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

        result = total;
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