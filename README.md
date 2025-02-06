# Card-project
The goal of this project is comparison of the effectiveness of different machine learning methods in creating a bot for card games.


# Blackjack
1. It uses standard 52 card deck
2. Cards 2 through 10 have their value set equal to their rank
3. Jacks, Queens and Kings have value equal to 10
4. Aces are worth either 1 or 11, depending on which one is in favour of the player
5. The goal of the game is to achieve cards with value adding up to 21 or as close to 21 as possible without exceding this value

Each round starts by each player placing a bet and the game is run by a dealer. The dealer deals one card face-up to each player and themself, starting from dealer's left and going to the right. Everyone is dealt one more face-up card besides the dealer, whose second card is dealt face-down.
You automatically win if you hit exactly 21, you get 1.5x your starting bet and are done for that round.
Each player, starting from dealer's left, can either "hit" to get another card or "stay" keeping their hand and passing their turn to the next player. You have no maximum hand size so you can hit multiple times, but you automatically lose if the next card you draw makes the total sum of your cards exceed 21 and the dealer gets your bet. This is called a "bust".
After each player's turn dealer reveals their second card. If their hand value is 16 or under, they have to hit, if it is 17 or higher, they have to stay. If the dealer busts, every player that hasn't busted in this round gets 2x their bet. If the dealer doesn't bust, only players with hands higher than the dealer's win 2x their bet and everyone else loses.
Depending on the rules adopted at the table during your turn you can also "double", doubling your bet but only drawing a single card and ending your turn, "split", if you get two cards of the same value you can make two hands out of them, or "surrender" which makes you lose but only giving the dealer half of your starting bet.

# Tested Methods
1. Genetic Algorithm – An optimization technique inspired by natural selection. It evolves solutions over generations using selection, crossover, and mutation to find optimal or near-optimal solutions.

2. Tabu Search – A metaheuristic algorithm that explores the solution space while avoiding recently visited solutions (stored in a "tabu list") to escape local optima and find better solutions.

3. Q-Learning – A reinforcement learning algorithm that uses a Q-table to estimate the optimal action-value function. It updates values based on rewards and future estimates to learn an optimal policy.

4. Monte Carlo Control – A reinforcement learning method that estimates action values based on averaged returns from sampled episodes, gradually improving policy performance over time.