#ifndef Q_LEARNING
#define Q_LEARNING

#include "bot_utils.hpp"

class q_learning_bot {
private:
    std::map<game_state, std::map<bot_move, double>> q_table;
    float alpha; // Learning rate
    float gamma; // Discount factor
    float epsilon; // Exploration rate

    int number_of_players;
    int current_player_index;
    int split_hand_offset;

    bool game_finished;

    // Dealer's hand on last index;
    std::vector<hand> hands;

    deck game_deck;

    void update_q_table(const game_state& state, bot_move action, float reward, const game_state& next_state) {
        double max_future_q = std::numeric_limits<double>::lowest();
        
        for (const auto& [next_action, next_q_value] : q_table[next_state]) {
            max_future_q = std::max(max_future_q, next_q_value);
        }

        double current_q_value = q_table[state][action];
        q_table[state][action] = current_q_value + alpha * (reward + gamma * max_future_q - current_q_value);
    }

    bot_move choose_action_learning(const game_state& state) {
        auto [player_points, dealer_card, has_ace, can_split, can_double_down] = state;

        std::vector<bot_move> valid_moves = {HIT, STAND};
        if (can_split) valid_moves.push_back(SPLIT);
        if (can_double_down) valid_moves.push_back(DOUBLE_DOWN);


        if ((rand() / static_cast<double>(RAND_MAX)) < epsilon) {
            return valid_moves[rand() % valid_moves.size()];
        } else {
            if (q_table.find(state) == q_table.end()) {
                std::cerr << "Error: State not found in q_table: (" 
                        << std::get<0>(state) << ", " 
                        << std::get<1>(state) << ", " 
                        << std::get<2>(state) << ", " 
                        << std::get<3>(state) << ", " 
                        << std::get<4>(state) << ")" << std::endl;
            }

            double max_q = std::numeric_limits<double>::lowest();
            bot_move best_move = HIT;
            for (const auto& [action, q_value] : q_table.at(state)) {
                if (q_value > max_q) {
                    max_q = q_value;
                    best_move = action;
                }
            }
            return best_move;
        }
    }

    bot_move choose_action(const game_state& state) {
        if (q_table.find(state) == q_table.end()) {
                std::cerr << "Error: State not found in q_table: (" 
                        << std::get<0>(state) << ", " 
                        << std::get<1>(state) << ", " 
                        << std::get<2>(state) << ", " 
                        << std::get<3>(state) << ", " 
                        << std::get<4>(state) << ")" << std::endl;
            }

            double max_q = std::numeric_limits<double>::lowest();
            bot_move best_move = HIT;
            for (const auto& [action, q_value] : q_table.at(state)) {
                if (q_value > max_q) {
                    max_q = q_value;
                    best_move = action;
                }
            }
            return best_move;
    }

    void deal_cards() {
        card pulled;

        // Add two card to each player's hand, dealer's hand is last
        for (int i = 0; i < 2; i++)
        {
            for (hand& hand : hands) {
                pulled = game_deck.pull_card();

                hand.add_card(pulled);
            }
        }
    }

public:
    q_learning_bot(float learning_rate, float discount_factor, float exploration_rate, const int players){
        alpha = learning_rate;
        gamma = discount_factor;
        epsilon = exploration_rate;

        number_of_players = players;
        current_player_index = 0;
        split_hand_offset = 0;

        game_deck.prepare_deck();
    }

    ~q_learning_bot() {}

    // We need more states for "invalid" moves that results in player busting
    void initialize_q_table() {
        for (int player_points = 2; player_points <= 30; ++player_points) {
            for (int dealer_points = 2; dealer_points <= 11; ++dealer_points) {
                for (bool has_ace : {true, false}) {
                    for (bool can_split : {true, false}) {
                        for (bool can_double_down : {true, false}) {
                            if(!can_double_down){
                                can_split = false;
                            }

                            game_state state{player_points, dealer_points, has_ace, can_split, can_double_down};

                            q_table[state] = {
                                {HIT, 0.0}, 
                                {STAND, 0.0}, 
                                {SPLIT, 0.0},
                                {DOUBLE_DOWN, 0.0}
                            };
                        }
                    }
                }
            }
        }
    }

    void prepare_game(){
        hands.clear();

        for(int i = 0; i < number_of_players - 1; i++){
            hands.emplace_back(PLAYER);
        }
        hands.emplace_back(DEALER);

        game_deck.prepare_deck();
        deal_cards();

        game_finished = false;
        current_player_index = 0;
        split_hand_offset = 0;
    }

    void set_finished(const bool is_finished){
        this->game_finished = is_finished;
    }

    hand get_dealer(){        
        return hands.back();
    }

    hand get_current_hand(){
        return hands[current_player_index + split_hand_offset];
    }

    int get_current_points(){
        return hands[current_player_index + split_hand_offset].get_points();
    }

    hand get_just_splitted(){
        return hands[current_player_index + split_hand_offset + 1];
    }

    void pass_turn(){
        if(hands[current_player_index + split_hand_offset + 1].get_is_split()){
            split_hand_offset += 1;
        }
        else {
            current_player_index += 1;
        }

        if(current_player_index + split_hand_offset + 2 > hands.size()){
            draw_for_dealer();
        }
    }

    card pull_for_current(){
        card pulled = game_deck.pull_card();
        int new_value;

        hands[current_player_index + split_hand_offset].add_card(pulled);

        return pulled;
    }

    void draw_for_dealer(){
        card pulled;

        hands.back().update_for_dealer();

        // Dealer stands on soft 17
        // Game variant
        while(hands.back().get_points() <= 16){
            pulled = game_deck.pull_card();

            hands.back().add_card(pulled);
        }

        this->game_finished = true;
    }

    void split_current_hand(){
        hand split_hand = hands[current_player_index + split_hand_offset].get_split();
        
        if (current_player_index + split_hand_offset >= 0 && current_player_index + split_hand_offset < hands.size()) {
            hands.insert(hands.begin() + current_player_index + split_hand_offset + 1, split_hand);
        }
    }

    bool get_game_finished(){
        return this->game_finished;
    }

    std::map<game_state, bot_move> extract_best_strategy() {
        std::map<game_state, bot_move> best_strategy;

        for (const auto& [state, moves] : q_table) {
            bot_move best_move = HIT; // Default move
            double max_q_value = std::numeric_limits<double>::lowest();

            for (const auto& [move, q_value] : moves) {
                if (q_value > max_q_value) {
                    max_q_value = q_value;
                    best_move = move;
                }
            }

            best_strategy[state] = best_move;
        }

        return best_strategy;
    }

    int calc_training_reward() {
        int total_return = 0;
        int factor = 1;

        for (size_t i = 0; i < hands.size() - 1; ++i) {
            if(hands[i].get_is_doubled()){
                factor = 2;
            }
            else{
                factor = 1;
            }

            if (hands.back().get_points() > 21) {  // Dealer busts
                if (hands[i].get_points() <= 21) {
                    total_return += 1 * factor;  // Win when dealer busts
                } else {
                    total_return -= 1 * factor;  // Player also busts
                }
            } else {  // Dealer does not bust
                if (hands[i].get_points() > hands.back().get_points() && hands[i].get_points() <= 21) {
                    total_return += 1 * factor;  // Player beats dealer
                } else if (hands[i].get_points() == hands.back().get_points() && hands[i].get_points() <= 21) {
                    total_return += 0;  // Push (tie)
                } else {
                    total_return -= 1 * factor;  // Player loses
                }
            }
        }

        return total_return;  // Allow negative rewards to reflect losses
    }

    int check_win_condition(){
        int total_return = 0;
        int factor = 1;

        for (size_t i = 0; i < hands.size() - 1; ++i) {
            if(hands[i].get_is_doubled()){
                factor = 2;
            }
            else{
                factor = 1;
            }

            if(hands.back().get_points() > 21) {
                if(hands[i].get_points() <= 21) {
                    total_return += 1 * factor;
                }
                else{
                    total_return -= 1 * factor;
                }
            } 
            else{
                if(hands[i].get_points() > hands.back().get_points() && hands[i].get_points() <= 21){
                    total_return += 1 * factor;
                }
                else if(hands[i].get_points() == hands.back().get_points() && hands[i].get_points() <= 21){
                    total_return += 0;
                } 
                else if(hands[i].check_for_blackjack()){
                    total_return += 0.5 * factor;
                }
                else{
                    total_return -= 1 * factor;
                }
            }

        }

        return std::max(0, total_return);
    }

    void train_bot(){
        game_state previous_state;
        game_state next_state;
        bot_move action;
        float reward = 0;

        int curr_points;

        int rounds_passed = 0;
        bool game_finished = false;
        bool splittable = false;

        game_state state;
        bot_move move;

        card pulled;
        hand just_splitted;

        prepare_game();

        while (true)
        {
            reward = 0;

            game_finished = get_game_finished();
            splittable = get_current_hand().check_for_split() && rounds_passed == 0;
            
            if(game_finished){
                reward = calc_training_reward();
                
                update_q_table(previous_state, action, reward, next_state);

                break;
            }

            if(get_current_hand().check_for_blackjack() && rounds_passed == 0){
                pass_turn();

                rounds_passed = 0;

                break;
            }

            state = {get_current_hand().get_points(), get_dealer().get_points(), get_current_hand().contains_ace(), splittable, rounds_passed == 0};
            move = choose_action_learning(state);

            previous_state = state;
            action = move;

            // Penalize the illegal move and continue
            if((move == SPLIT && !splittable) || (move == DOUBLE_DOWN && rounds_passed != 0)){
                reward = -10.0;
                update_q_table(previous_state, action, reward, state); // State doesn't change
                continue;
            }

            if(move == HIT){
                pulled = pull_for_current();
                curr_points = get_current_points();

                rounds_passed += 1;

                next_state = {curr_points, get_dealer().get_points(), get_current_hand().contains_ace(), false, false};


                if(curr_points < 21){
                    if(curr_points <= 17){
                        reward = 0.1;
                    }
                    else if(curr_points <= 19){
                        reward = 0.2;
                    }
                    else{
                        reward = 0.3;
                    }
                }
                else if(curr_points == 21){
                    reward = 2;

                    pass_turn();
                    rounds_passed = 0;
                }
                else if(curr_points > 21){
                    reward = -3;

                    pass_turn();
                    rounds_passed = 0;
                }

                update_q_table(previous_state, action, reward, next_state);
            }
            else if(move == STAND){
                pass_turn();

                next_state = previous_state;

                rounds_passed = 0;
            }
            else if(move == DOUBLE_DOWN){
                pulled = pull_for_current();
                curr_points = get_current_points();
                get_current_hand().double_bet();

                next_state = {curr_points, get_dealer().get_points(), get_current_hand().contains_ace(), false, false};

                if (get_current_points() > 21) {
                    reward = -3;
                } 
                else if(curr_points == 21) {
                    reward = 2;
                }

                if(reward != 0){
                    update_q_table(previous_state, action, reward, next_state);
                }

                pass_turn();
                rounds_passed = 0;
            }
            else if(move == SPLIT){
                split_current_hand();

                reward = 0;

                just_splitted = get_just_splitted();
                next_state = {just_splitted.get_points(), get_dealer().get_points(), just_splitted.contains_ace(), false, false};

                update_q_table(previous_state, action, reward, next_state);

                curr_points = get_current_points();
                next_state = {curr_points, get_dealer().get_points(), get_current_hand().contains_ace(), false, false};

                update_q_table(previous_state, action, reward, next_state);



                rounds_passed += 1;
            }

        }
    }

    int run_game(){
        int rounds_passed = 0;
        bool game_finished = false;
        bool splittable = false;

        game_state state;
        bot_move move;

        card pulled;

        prepare_game();

        while (true)
        {
            game_finished = get_game_finished();
            splittable = get_current_hand().check_for_split() && rounds_passed == 0;
            
            if(game_finished){
                int result = check_win_condition();
                return result;
            }

            if(get_current_hand().check_for_blackjack() && rounds_passed == 0){
                pass_turn();

                rounds_passed = 0;

                continue;
            }

            if(get_current_hand().get_is_split() && rounds_passed == 0){
                rounds_passed += 1;
            }

            state = {get_current_hand().get_points(), get_dealer().get_points(), get_current_hand().contains_ace(), splittable, rounds_passed == 0};
            move = choose_action(state);

            if(move == HIT){
                pulled = pull_for_current();

                rounds_passed += 1;

                if(get_current_points() >= 21){
                    pass_turn();

                    rounds_passed = 0;
                }
            }
            else if(move == STAND){
                pass_turn();

                rounds_passed = 0;
            }
            else if(move == DOUBLE_DOWN){
                pulled = pull_for_current();
                get_current_hand().double_bet();

                pass_turn();

                rounds_passed = 0;
            }
            else if(move == SPLIT){
                split_current_hand();

                rounds_passed += 1;
            }

        }
    }

};


#endif