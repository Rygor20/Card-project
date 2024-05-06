#include "deck.hpp"

class game_handler
{
private:
    std::vector<int> player_points;

public:
    game_handler() {}

    ~game_handler() {}

    void add_players(std::vector<std::vector<card>>& hands){
        int hand_value;

        for(std::vector<card>& hand : hands) {
            // Get starting hands and initialise starting game state
        }
    }

    void update_player(std::vector<card> hand, int position){
        // From hand update game state for player at given position
    }
};