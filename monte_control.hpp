#ifndef MONTE_CONTROL
#define MONTE_CONTROL

#include <tuple>
#include <map>
#include <algorithm>
#include <random>
#include "deck.hpp"

std::mt19937 gen(rd());

// {Player Points, Dealer Card, Has usable Ace?}
using monte_game_state = std::tuple<int, int, bool>;

enum monte_bot_move {
    M_HIT, M_STAND
};

monte_bot_move sample_action(const std::vector<float>& probs) {
    std::discrete_distribution<> dist(probs.begin(), probs.end());
    return static_cast<monte_bot_move>(dist(gen));
}

monte_bot_move sample_random_action() {
    std::uniform_int_distribution<> dist(0, 1); // 0 for HIT, 1 for STAND
    return static_cast<monte_bot_move>(dist(gen));
}

struct game_simulation
{
    deck game_deck;
    std::vector<hand> hands;
    int number_of_players;

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

    void prepare_game(int num_players){
        number_of_players = num_players;

        hands.clear();

        for(int i = 0; i < number_of_players - 1; i++){
            hands.emplace_back(PLAYER);
        }
        hands.emplace_back(DEALER);

        game_deck.prepare_deck();
        deal_cards();
    }

    monte_game_state game_status(){
        monte_game_state state{hands[0].get_points(), hands[1].get_upcard().get_value(), hands[0].contains_ace()};
        return state;
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
    }

    float calculate_reward(){
        float reward = 0;
        int player_points = hands[0].get_points();

        if(hands.back().get_points() > 21) {
            if(player_points <= 21) {
                // Dealer busts, player not
                reward = 1;
            } else {
                // Dealer and player bust
                reward = -1;
            }
        } else {
            if(player_points > hands.back().get_points() && player_points <= 21) {
                // No busts, player hand wins
                reward = 1;
            }
            else if(player_points == hands.back().get_points() && player_points <= 21) {
                // No busts, hands are equal
                reward = 0;
            } 
            else if(hands[0].check_for_blackjack()){
                // No busts, blackjack in initial hand and no blackjack in dealer's hand
                reward = 0.5;
            }
            else {
                // No busts, player loses
                reward = -1;
            }
        }

        return reward;
    }

    std::tuple<monte_game_state, int, bool> game_step(monte_bot_move action){
        float reward = 0.0;
        bool finished = false;

        card pulled;

        if(action == M_HIT){
            pulled = game_deck.pull_card();
            hands[0].add_card(pulled);

            if(hands[0].get_points() > 21){
                reward = -1.0;
                finished = true;
            }
        }
        else if(action == M_STAND){
            finished = true;

            draw_for_dealer();

            reward = calculate_reward();
        }

        monte_game_state state = game_status();
        return {state, reward, finished};
    }
};


std::vector<float> get_probs(std::map<monte_bot_move, float> Q_values, float epsilon, int num){
    std::vector<float> policy(num, epsilon / num);

    auto best_action = Q_values.begin();  // Initialize best_action to the first element of Q_values

    // Iterate through the map to find the action with the highest Q-value
    for (auto it = Q_values.begin(); it != Q_values.end(); ++it) {
        if (it->second > best_action->second) {  // Compare Q-values (second element of the pair)
            best_action = it;  // Update best_action to the new element with the higher Q-value
        }
    }

    // Adjust the probability of the best action for exploitation
    policy[static_cast<int>(best_action->first)] = 1 - epsilon + (epsilon / num);

    return policy;
}

std::map<monte_game_state, monte_bot_move> best_policy(const std::map<monte_game_state, std::map<monte_bot_move, float>>& Q) {
    std::map<monte_game_state, monte_bot_move> policy;
    
    // Iterate through each monte_game_state in Q
    for (const auto& entry : Q) {
        const monte_game_state& state = entry.first;
        const std::map<monte_bot_move, float>& action_values = entry.second;
        
        // Initialize variables to track the best action and its value
        monte_bot_move best_action;
        float max_value = -std::numeric_limits<float>::infinity();  // Ensure any Q value is larger than this
        
        // Iterate through the actions and find the one with the highest Q-value
        for (const auto& action_value : action_values) {
            const monte_bot_move& action = action_value.first;
            float value = action_value.second;
            
            // If the current Q-value is larger than the max_value, update the best_action
            if (value > max_value) {
                best_action = action;
                max_value = value;
            }
        }
        
        // Store the best action for this state in the policy
        policy[state] = best_action;
    }
    
    return policy;
}

void update_Q(std::map<monte_game_state, std::map<monte_bot_move, float>>& Q, const std::vector<std::tuple<monte_game_state, monte_bot_move, float>>& episode, float alpha, float gamma) {
    // Iterate over the episode
    for (size_t idx = 0; idx < episode.size(); ++idx) {
        const monte_game_state& state = std::get<0>(episode[idx]);
        const monte_bot_move& action = std::get<1>(episode[idx]);
        float reward = std::get<2>(episode[idx]);
        
        // Find the first occurrence index of this state in the episode
        size_t first_occurence_idx = idx;
        for (size_t i = 0; i < idx; ++i) {
            if (std::get<0>(episode[i]) == state) {
                first_occurence_idx = i;
                break;
            }
        }
        
        // Calculate G: the discounted sum of rewards starting from the first occurrence
        float G = 0.0f;
        for (size_t i = first_occurence_idx; i < episode.size(); ++i) {
            G += std::get<2>(episode[i]) * std::pow(gamma, i - first_occurence_idx);
        }
        
        // Update Q-value using the formula Q(s, a) = Q(s, a) + alpha * (G - Q(s, a))
        Q[state][action] += alpha * (G - Q[state][action]);
    }
}

std::vector<std::tuple<monte_game_state, monte_bot_move, float>> play_game(std::map<monte_game_state, std::map<monte_bot_move, float>> Q, float epsilon, int num){
    game_simulation simulation;
    simulation.prepare_game(2);

    std::vector<std::tuple<monte_game_state, monte_bot_move, float>> episode;
    monte_game_state state = simulation.game_status();
    std::vector<float> probabilities;
    
    monte_bot_move action;
    while(true){
        if (Q.find(state) != Q.end()) {
            // State exists, get probabilities and sample action
            probabilities = get_probs(Q[state], epsilon, num);
            action = sample_action(probabilities);
        } else {
            // State not in Q, take a random action
            action = sample_random_action();
        }

        auto [next_state, reward, finished] = simulation.game_step(action);

        episode.push_back({state, action, reward});

        if (finished) {
            break;
        }

        state = next_state;
    }

    return episode;
}

void test_policy(std::map<monte_game_state, monte_bot_move> policy, int num_games) {
    int wins = 0;
    int losses = 0;
    int ties = 0;

    for (int i = 0; i < num_games; ++i) {
        game_simulation simulation;
        simulation.prepare_game(2);

        monte_game_state state = simulation.game_status();
        bool finished = false;
        float reward = 0.0;

        // Run the game with the given policy
        while (!finished) {
            monte_bot_move action;
            if (policy.find(state) != policy.end()) {
                action = policy.at(state);  // Get the action from the policy
            } else {
                action = sample_random_action();  // Default to random action if state is not in policy
            }

            // Take the action in the simulation and get the next state, reward, and whether the game is finished
            auto [next_state, step_reward, finished_flag] = simulation.game_step(action);
            state = next_state;
            reward = step_reward;
            finished = finished_flag;
        }

        // Evaluate the final reward (1 for win, -1 for loss, 0 for tie)
        if (reward == 1.0) {
            ++wins;
        } else if (reward == -1.0) {
            ++losses;
        } else {
            ++ties;
        }
    }

    // Calculate the winrate
    float winrate = static_cast<float>(wins) / num_games;
    std::cout << "Winrate: " << winrate * 100 << "%" << std::endl;
    std::cout << "Wins: " << wins << ", Losses: " << losses << ", Ties: " << ties << std::endl;
}

std::map<monte_game_state, monte_bot_move> mc_control(int episodes){
    float epsilon = 1.0;
    float epsilon_minimum = 0.01;
    float decay = 0.9999;
    float alpha = 0.001;
    float gamma = 1.0;

    std::map<monte_game_state, std::map<monte_bot_move, float>> Q;
    std::vector<std::tuple<monte_game_state, monte_bot_move, float>> episode;

    for(int i = 0; i < episodes; i++){
        int percent = static_cast<int>((static_cast<float>(i) / episodes) * 100);
        if (i % 1000 == 0) {
            std::cout << "\rProgress: " << percent << "% (" << i << "/" << episodes << ")"
                    << std::flush;
        }

        epsilon = std::max(epsilon * decay, epsilon_minimum);

        episode = play_game(Q, epsilon, 2);
        update_Q(Q, episode, alpha, gamma);
    }

    std::cout << "Generating policy finished" << std::endl;

    return best_policy(Q);
}


#endif