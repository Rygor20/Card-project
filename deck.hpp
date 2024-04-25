#include <vector>

enum card_suits {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

enum card_colour {
    BLACK, RED
};

enum card_rank {
    ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING
};

class card
{
private:
    const static card_suits suit;
    const static card_colour colour;
    const static card_rank rank;
    
public:
    card(/* args */);
    ~card();
};

card::card(/* args */)
{
}

card::~card()
{
}

class deck
{
private:
    const static int starting_size;
    static int current_size;

    std::vector<card> cards;

public:
    deck(/* args */);
    ~deck();
};

deck::deck(/* args */)
{
}

deck::~deck()
{
}
