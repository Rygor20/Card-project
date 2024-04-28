#include <vector>
#include <algorithm> 
#include <random> 

std::random_device rd; 
std::mt19937 twister(rd()); 

enum card_suit {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

enum card_colour {
    BLACK, RED
};

enum card_rank {
    TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
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
        std::uniform_int_distribution<int> range(0, current_size);
        int rand_int = range(twister);

        card pulled_card = cards[rand_int];

        cards.erase(cards.begin() + rand_int);
        current_size -= 1;

        return pulled_card;
    }
};
