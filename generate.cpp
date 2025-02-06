#include <iostream>
#include <fstream>
#include "meta_genetic.hpp"
#include "q_learning.hpp"
#include "monte_control.hpp"


std::string monte_bot_move_to_string(monte_bot_move move) {
    switch (move) {
        case M_HIT: return "HIT";
        case M_STAND: return "STAND";
        default: return "UNKNOWN";
    }
}

void write_policy_to_file(const std::map<monte_game_state, monte_bot_move>& policy, const std::string& filename) {
    std::ofstream file(filename);

    // Check if file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write the usable ace table
    file << "Policy Table (Usable Ace)\n";
    file << "Player Points \\ Dealer Upcard | ";
    for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
        file << dealer_card << " ";
    }
    file << "\n";

    // Iterate over player points (rows) and dealer upcard (columns)
    for (int player_points = 4; player_points <= 20; ++player_points) {
        for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
            monte_game_state state = std::make_tuple(player_points, dealer_card, true); // Usable ace case

            file << player_points << " \\ " << dealer_card << " | ";
            if (policy.find(state) != policy.end()) {
                file << monte_bot_move_to_string(policy.at(state)) << " ";
            } else {
                file << "UNKNOWN ";
            }
        }
        file << "\n";
    }

    file << "\n\n";

    // Write the non-usable ace table
    file << "Policy Table (No Usable Ace)\n";
    file << "Player Points \\ Dealer Upcard | ";
    for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
        file << dealer_card << " ";
    }
    file << "\n";

    // Iterate over player points (rows) and dealer upcard (columns)
    for (int player_points = 4; player_points <= 20; ++player_points) {
        for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
            monte_game_state state = std::make_tuple(player_points, dealer_card, false); // No usable ace case

            file << player_points << " \\ " << dealer_card << " | ";
            if (policy.find(state) != policy.end()) {
                file << monte_bot_move_to_string(policy.at(state)) << " ";
            } else {
                file << "UNKNOWN ";
            }
        }
        file << "\n";
    }

    file.close();  // Close the file
    std::cout << "Policy has been written to " << filename << std::endl;
}

std::string bot_move_to_string(bot_move move) {
    switch (move) {
        case HIT: return "HIT";
        case STAND: return "STAND";
        case SPLIT: return "SPLIT";
        case DOUBLE_DOWN: return "DOUBLE_DOWN";
        default: return "UNKNOWN";
    }
}

std::string combine_moves(const std::string& move1, const std::string& move2) {
    if (move1 == "HIT" && move2 == "HIT") return "HIT";
    if (move1 == "STAND" && move2 == "STAND") return "STAND";
    if (move1 == "DOUBLE_DOWN" && move2 == "DOUBLE_DOWN") return "DD";
    if ((move1 == "DOUBLE_DOWN" && move2 == "HIT") || (move1 == "HIT" && move2 == "DOUBLE_DOWN")) return "DH";
    if ((move1 == "DOUBLE_DOWN" && move2 == "STAND") || (move1 == "STAND" && move2 == "DOUBLE_DOWN")) return "DS";
    if ((move1 == "HIT" && move2 == "STAND") || (move1 == "STAND" && move2 == "HIT")) return (std::rand() % 2 == 0) ? "HIT" : "STAND";

    std::cout << "move1: " << move1 << " move2: " << move2 << std::endl;
    return "UNKNOWN";
}

void write_strategy_to_file(const std::map<game_state, bot_move>& policy, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write the usable ace table
    file << "Strategy Table (Usable Ace)\n";
    file << "Player Points \\ Dealer Upcard | ";
    for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
        file << dealer_card << " ";
    }
    file << "\n";

    for (int player_points = 4; player_points <= 20; ++player_points) {
        for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
            // Wczytanie wyników dla obu wartości can_double_down
            std::string move_with_true = "UNKNOWN";
            std::string move_with_false = "UNKNOWN";

            for (bool can_double_down : {true, false}) {
                game_state state = std::make_tuple(player_points, dealer_card, true, false, can_double_down); // Usable ace, no split

                if (policy.find(state) != policy.end()) {
                    if (can_double_down) {
                        move_with_true = bot_move_to_string(policy.at(state));
                    } else {
                        move_with_false = bot_move_to_string(policy.at(state));
                    }
                }
            }

            // Zastosowanie funkcji do połączenia wyników
            file << player_points << " \\ " << dealer_card << " | "
                << combine_moves(move_with_true, move_with_false) << " ";
        }
        file << "\n";
    }

    file << "\n\n";

    // Write the non-usable ace table
    file << "Strategy Table (No Usable Ace)\n";
    file << "Player Points \\ Dealer Upcard | ";
    for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
        file << dealer_card << " ";
    }
    file << "\n";

    for (int player_points = 4; player_points <= 20; ++player_points) {
        for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
            // Wczytanie wyników dla obu wartości can_double_down
            std::string move_with_true = "UNKNOWN";
            std::string move_with_false = "UNKNOWN";

            for (bool can_double_down : {true, false}) {
                game_state state = std::make_tuple(player_points, dealer_card, false, false, can_double_down); // No usable ace, no split

                if (policy.find(state) != policy.end()) {
                    if (can_double_down) {
                        move_with_true = bot_move_to_string(policy.at(state));
                    } else {
                        move_with_false = bot_move_to_string(policy.at(state));
                    }
                }
            }

            // Zastosowanie funkcji do połączenia wyników
            file << player_points << " \\ " << dealer_card << " | "
                << combine_moves(move_with_true, move_with_false) << " ";
        }
        file << "\n";
    }

    file << "\n\n";

    for (int player_points = 4; player_points <= 20; ++player_points) {
        if(player_points % 2 == 0){
            for (int dealer_card = 2; dealer_card <= 11; ++dealer_card) {
                bool found_split = false; // Flaga, która sprawdza, czy SPLIT wystąpił w dowolnym przypadku

                for (bool has_usable_ace : {true, false}) {
                    game_state state = std::make_tuple(player_points, dealer_card, has_usable_ace, true, true); // Split i can_double_down == true

                    if (policy.find(state) != policy.end() && policy.at(state) == SPLIT) {
                        found_split = true; // Jeśli SPLIT znaleziono, ustawiamy flagę
                        break; // Nie musimy dalej szukać, wystarczy jeden SPLIT
                    }
                }

                // Dodanie wpisu do tabeli
                file << player_points << " \\ " << dealer_card << " | "
                    << (found_split ? "Y" : "N") << " ";
            }
            file << "\n";
        }
    }

    file.close();  // Close the file
    std::cout << "Strategy has been written to " << filename << std::endl;
}


int main(int argc, char const *argv[])
{
    //Tabu Search
    std::vector<std::map<game_state, bot_move>> population = initialize_population(1);
    std::map<game_state, bot_move> strategy = population[0];

    float eval = evaluate_strategy(strategy, 25000);
    std::cout << "Fitness before tabu: " << eval << std::endl;

    std::map<game_state, bot_move> tabu_result = tabu_search(strategy, 0.1, 0.2);
    eval = evaluate_strategy(tabu_result, 25000);
    std::cout << "Fitness after tabu: " << eval << std::endl;

    write_strategy_to_file(tabu_result, "tabu_result.txt");


    // Genetic Algorithm (or memetic, if the relevant code fragment has been uncommented)
    std::map<game_state, bot_move> strategy = run_genetic_algorithm(200, 100, 0.85, 0.1, 0.3);

    float total_score = 0.f;
    float best_score = 0.f;
    float worst_score = 1.f;
    float fitness_score;
    
    for(int i = 0; i < 100; i++){
        fitness_score = evaluate_strategy(strategy, 25000);
        total_score += fitness_score;

        if(fitness_score > best_score){
            best_score = fitness_score;
        }
        if(fitness_score < worst_score){
            worst_score = fitness_score;
        }
    }

    std::cout << "Strategy size: " << strategy.size() << std::endl;

    std::cout << "Best score encountered: " << best_score << std::endl;
    std::cout << "Worst score encountered: " << worst_score << std::endl;
    std::cout << "Final Fitness: " << std::to_string(total_score / 100) << std::endl;

    write_strategy_to_file(strategy, "genetic_result.txt");
    //write_strategy_to_file(strategy, "memetic_result.txt");


    // Q-Learning
    float alpha = 0.1;
    float gamma = 0.9;
    float epsilon = 0.1;

    float result = 0.f, total = 0.f;

    q_learning_bot bot = q_learning_bot(alpha, gamma, epsilon, 2);

    bot.initialize_q_table();

    int reps = 20000000;
    int test_reps = 200000;
    for(int i = 0; i < reps; i++){
        bot.train_bot();

        if (i % (reps / 100) == 0) { // Print for every 1% completion
            std::cout << "Progress: " << (i * 100) / reps << "% completed\r";
            std::cout.flush();
        }
    }

    for(int i = 0; i < test_reps; i++){
        result = bot.run_game();
        total += result;
    }
    
    std::cout << "Q-Learning win rate: " << total << " / " << test_reps << std::endl;
    std::cout << "Q-Learning win rate: " << std::to_string(total / test_reps) << std::endl;

    std::map<game_state, bot_move> strategy = bot.extract_best_strategy();
    write_strategy_to_file(strategy, "q_result.txt");


    // Monte Carlo Control
    std::map<monte_game_state, monte_bot_move> policy = mc_control(5000000);
    test_policy(policy, 200000);

    write_policy_to_file(policy, "monte_result.txt");



    // int population_size = 200;

    // std::cout << "1" << std::endl;
    // std::vector<std::map<game_state, bot_move>> population = initialize_population(population_size);
    // std::cout << "2" << std::endl;
    // std::vector<float> fitness_scores = evaluate_population_async(population, 15000);
    // std::cout << "3" << std::endl;

    // float best = 0.f;
    // float worst = 1.f;
    // float total = 0.f;

    // std::cout << "4" << std::endl;
    // for (const float score : fitness_scores) {
    //     total += score;

    //     if(score > best){
    //         best = score;
    //     }
    //     if(score < worst){
    //         worst = score;
    //     }
    // }

    // std::cout << "Best: " << std::to_string(best) << std::endl;
    // std::cout << "Worst: " << std::to_string(worst) << std::endl;
    // std::cout << "Average: " << std::to_string(total / population_size) << std::endl;

    return 0;
}
