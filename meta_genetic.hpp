#ifndef GENETIC_HEADER
#define GENETIC_HEADER

#include <iostream>
#include <vector>
#include <map>
#include <future>
#include "deck.hpp"
#include "bot_utils.hpp"
#include "local_algorithms.hpp"

std::vector<std::map<game_state, bot_move>> initialize_population(int population_size) {
    std::vector<std::map<game_state, bot_move>> population;

    for (int i = 0; i < population_size; ++i) {
        std::map<game_state, bot_move> strategy;

        for (int player_points = 2; player_points <= 21; ++player_points) {
            for (int dealer_points = 2; dealer_points <= 11; ++dealer_points) {
                for (bool has_ace : {true, false}) {
                    for (bool can_split : {true, false}) {
                        for (bool can_double_down : {true, false}) {
                            if(!can_double_down){
                                can_split = false;
                            }

                            game_state state{player_points, dealer_points, has_ace, can_split, can_double_down};

                            // Determine valid moves
                            std::vector<bot_move> valid_moves = {HIT, STAND};
                            if (can_split) valid_moves.push_back(SPLIT);
                            if (can_double_down) valid_moves.push_back(DOUBLE_DOWN);

                            // Randomly pick one of the valid moves
                            bot_move random_move = valid_moves[rand() % valid_moves.size()];

                            strategy[state] = random_move;                        
                        }
                    }
                }
            }
        }
        population.push_back(strategy);
    }

    return population;
}

// Single-point crossover
std::pair<std::map<game_state, bot_move>, std::map<game_state, bot_move>> 
crossover(const std::map<game_state, bot_move>& parent1, const std::map<game_state, bot_move>& parent2, float crossover_rate) {
    if ((rand() / static_cast<float>(RAND_MAX)) > crossover_rate) {
        return {parent1, parent2};
    }

    std::map<game_state, bot_move> child1, child2;
    auto it1 = parent1.begin();
    auto it2 = parent2.begin();
    size_t split_point = rand() % parent1.size();
    size_t index = 0;

    while (it1 != parent1.end()) {
        if (index < split_point) {
            child1[it1->first] = it1->second;
            child2[it2->first] = it2->second;
        } else {
            child1[it2->first] = it2->second;
            child2[it1->first] = it1->second;
        }
        ++it1;
        ++it2;
        ++index;
    }

    return {child1, child2};
}

// Tournament Selection
int select_parent(const std::vector<float>& fitness_scores, int tournament_size) {
    int population_size = fitness_scores.size();
    float best_fitness = -1.0f;
    int best_index = -1;

    for (int i = 0; i < tournament_size; ++i) {
        int random_index = rand() % population_size;
        if (fitness_scores[random_index] > best_fitness) {
            best_fitness = fitness_scores[random_index];
            best_index = random_index;
        }
    }

    return best_index;
}

// // Roulette Wheel Selection
// int select_parent(const std::vector<float>& fitness_scores) {
//     float total_fitness = std::accumulate(fitness_scores.begin(), fitness_scores.end(), 0.0f);

//     float random_value = static_cast<float>(rand()) / RAND_MAX * total_fitness;

//     float cumulative_fitness = 0.0f;
//     for (size_t i = 0; i < fitness_scores.size(); ++i) {
//         cumulative_fitness += fitness_scores[i];
//         if (random_value <= cumulative_fitness) {
//             return i;
//         }
//     }
//     std::cout << "Selected last index" << std::endl;
//     return fitness_scores.size() - 1;
// }

void optimize_population_with_tabu(std::vector<std::map<game_state, bot_move>>& population, double board_factor, double iterations_factor, size_t num_individuals_to_optimize) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<size_t> indices(population.size() - 1);
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<size_t> selected_indices;
    std::sample(indices.begin(), indices.end(), std::back_inserter(selected_indices), num_individuals_to_optimize, gen);

    std::vector<std::future<std::map<game_state, bot_move>>> futures;

    for (size_t i = 0; i < num_individuals_to_optimize; ++i) {
        size_t selected_index = selected_indices[i];
        futures.push_back(std::async(std::launch::async, tabu_search, population[selected_index], board_factor, iterations_factor));
    }

    for (size_t i = 0; i < num_individuals_to_optimize; ++i) {
        size_t selected_index = selected_indices[i];
        population[selected_index] = futures[i].get();
    }
}

std::map<game_state, bot_move> run_genetic_algorithm(int generations, int population_size, float cross_rate, float mutation_rate, float memetic_rate) {
    auto population = initialize_population(population_size);
    std::vector<float> fitness_scores;
    std::map<game_state, bot_move> parent1, parent2, child;
    std::vector<std::map<game_state, bot_move>> new_population;

    for (int generation = 0; generation < generations; ++generation) {
        fitness_scores = evaluate_population_async(population, 15000);
        new_population.clear();

        for (int i = 0; i < population_size / 2; ++i) {
            parent1 = population[select_parent(fitness_scores, population_size / 5)];
            parent2 = population[select_parent(fitness_scores, population_size / 5)];
            
            auto [child1, child2] = crossover(parent1, parent2, cross_rate);
            mutate(child1, mutation_rate);
            mutate(child2, mutation_rate);

            new_population.push_back(child1);
            new_population.push_back(child2);
        }

        // if ((rand() / static_cast<float>(RAND_MAX)) < memetic_rate) {
        //     // Memetic optimization
        //     size_t num_to_optimize = population_size / 5;
        //     optimize_population_with_tabu(new_population, 0.15, 0.02, num_to_optimize);
        // }

        population = std::move(new_population);
    }

    fitness_scores = evaluate_population_async(population, 25000);

    float best = 0.f;

    for (const float score : fitness_scores) {
        if(score > best){
            best = score;
        }
    }

    std::cout << "Best score: " << best << std::endl;

    return population[std::distance(fitness_scores.begin(), std::max_element(fitness_scores.begin(), fitness_scores.end()))];
}



#endif