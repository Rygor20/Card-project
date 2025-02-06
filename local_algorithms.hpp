#ifndef LOCAL_ALGORITHMS
#define LOCAL_ALGORITHMS

#include <deque>
#include "bot_utils.hpp"

struct tabu_list {
    std::deque<std::map<game_state, bot_move>> list;
    size_t max_size;

    tabu_list(size_t size) : max_size(size) {}

    bool is_tabu(const std::map<game_state, bot_move>& strategy) {
        for (const auto& s : list) {
            if (s == strategy) {
                return true;
            }
        }
        return false;
    }

    void add(const std::map<game_state, bot_move>& strategy) {
        if (list.size() == max_size) {
            list.pop_front();
        }
        list.push_back(strategy);
    }
};

std::map<game_state, bot_move> tabu_search(const std::map<game_state, bot_move>& initial_strategy, double board_factor, double iterations_factor) {
    const size_t n = initial_strategy.size();
    const size_t tabu_list_size = static_cast<size_t>(static_cast<double>(n) * board_factor);
    const size_t iterations = static_cast<size_t>(static_cast<double>(n) * iterations_factor);

    std::cout << "Max Tabu iteration: " << iterations << std::endl;

    tabu_list tabu_list(tabu_list_size);

    std::map<game_state, bot_move> current_strategy = initial_strategy;
    double current_fitness = evaluate_strategy(current_strategy, 25000);

    std::map<game_state, bot_move> best_strategy = current_strategy;
    double best_fitness = current_fitness;

    std::vector<std::map<game_state, bot_move>> neighbors;
    std::map<game_state, bot_move> neighbor_strategy;
    std::vector<float> neighbor_fitnesses;

    std::map<game_state, bot_move> best_neighbor;
    double best_neighbor_fitness;

    size_t it = 0;
    while (it < iterations) {
        it++;

        std::cout << "Current Tabu iteration: " << it << std::endl;

        best_neighbor = current_strategy;
        best_neighbor_fitness = current_fitness;

        // Generate neighbors and evaluate asynchronously
        neighbors.clear();
        for (size_t i = 0; i < n; ++i) {
            // Create a neighbor by mutation
            neighbor_strategy = current_strategy;
            mutate(neighbor_strategy, 0.05);  // Mutate 5% of the strategy
            neighbors.push_back(neighbor_strategy);
        }

        // Asynchronously evaluate all the neighbors at once
        neighbor_fitnesses = evaluate_population_async(neighbors, 25000);

        // Find the best neighbor that isn't in tabu list
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const auto& neighbor_strategy = neighbors[i];
            if (!tabu_list.is_tabu(neighbor_strategy) && neighbor_fitnesses[i] > best_neighbor_fitness) {
                best_neighbor = neighbor_strategy;
                best_neighbor_fitness = neighbor_fitnesses[i];
            }
        }

        // If the best neighbor is better than the current strategy, adopt it
        if (best_neighbor_fitness > current_fitness) {
            current_strategy = best_neighbor;
            current_fitness = best_neighbor_fitness;

            // Update the best strategy found so far
            if (current_fitness > best_fitness) {
                best_strategy = current_strategy;
                best_fitness = current_fitness;
                it = 0;  // Optionally reset the iteration count if a new best strategy is found
            }

            // Add the current strategy to the tabu list
            tabu_list.add(current_strategy);
        }
    }

    return best_strategy;
}


#endif