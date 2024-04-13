// AI.h
#pragma once

#include <iostream>
#include <cmath>
#include "../MySTL/vector.h"
#include "GameBoard.h"
#include "State.h"
// #include <concepts>

// template<typename T>
// concept GameConcept = requires(T game) {
//     { game.simulate(std::declval<typename T::STATE>()) } -> std::same_as<void>;
//     { T::MOVE } -> std::same_as<typename T::MOVE>;
//     { T::PLAYER_NOTATION } -> std::same_as<typename T::PLAYER_NOTATION>;
//     { T::STATE } -> std::same_as<typename T::STATE>;
// };

// Now, apply the concept as a constraint on the AI class template
template <typename GAME>
class AI {

private:
    //GAME* game is a pointer to the game object that the AI will play
    GAME* game;

    //AI Turn is the turn of the AI
    typename GAME::PLAYER_NOTATION AI_Turn;
    
    //exploration factor is the parameter that controls the exploration vs exploitation tradeoff in the UCB formula
    double exploration_factor = 0.5;


    //Function to simulate the game
    template<typename T,typename U>
    Vector<int> simulate_game(T move,U turn) {

        //make a move in the game
        game->simulate(move,turn);
        Vector<int> result(3, 0);
        Vector<typename GAME::MOVE> valid_moves = game->get_valid_moves();
        int terminal_state = game->get_game_state();
        //checking for terminal state
        if(terminal_state != -1){
            if(terminal_state == AI_Turn){
                result[0] = 1;
            }
            else if(terminal_state == -2){
                result[1] = 1;
            }
            else{
                result[2] = 1;
            }
            game->simulate(move,SIMULATE_STATE::UNMOVE);
            return result;
        }

        //checking for draw
        if(valid_moves.size() == 0){
            result[1] = 1;
            game->simulate(move,SIMULATE_STATE::UNMOVE);
            return result;
        }

        //Simulate the game for each valid move
        for (auto _move : valid_moves) {
            Vector<int> recursive_result = simulate_game(_move,game->get_next_player(turn));
            result[0] += recursive_result[0]; // Wins from recursive simulation
            result[1] += recursive_result[1]; // Draws from recursive simulation
            result[2] += recursive_result[2]; // Losses from recursive simulation
        }

        // Undo the move before returning
        game->simulate(move,SIMULATE_STATE::UNMOVE);
        return result;
    }

public:
    //Constructor for the AI class
    AI(GAME* game ) : game(game) {}

    //Function to set the AI's turn
    void set_turn(typename GAME::PLAYER_NOTATION turn) {
        AI_Turn = turn;
    }


    //Function to set the exploration factor
    void set_exploration_factor(double factor) {
        exploration_factor = factor;
    }

    //function that decides the move for the AI
    std::string decide_move()
    {
        //Simulate the current state of the game
        game->simulate(SIMULATE_STATE::SAVE_BOARD);  // overloaded function to save the current state of the game

        //Get the valid moves for the current state of the game
        Vector<typename GAME::MOVE> valid_moves = game->get_valid_moves();
        Vector<double> UBC_Rates;

        //Get the current player's notation
        typename GAME::PLAYER_NOTATION ai_notation = game->get_turn();
        int num_players = game->get_num_players();

        //If the game is over, return an empty string
        for(auto moves : valid_moves){

            // Simulate the game for each valid move and calculate the UCB value for each move
            Vector<int> result = simulate_game<typename GAME::MOVE,typename GAME::PLAYER_NOTATION>(moves,1);
            int total_games = result[0] + result[1] + result[2];

            cout<<"FOR MOVE " << moves << " WINS ARE " << result[0] << " DRAWS ARE " << result[1] << " LOSSES ARE " << result[2] <<"Tots :"<<total_games<<endl;

            //modified version of the UBC , change it according to prefs
            double UCB = ((result[0]*3.0 + result[1]*2.0 - result[2]*3.0)/total_games) + sqrt(2 * log(total_games) / (total_games * exploration_factor));
            cout<<"FOR MOVE " << moves << " UCB IS " << UCB << endl;
            UBC_Rates.push_back(UCB);
        }
        
        //get the best move based on the UBC values
        int max_index = 0 ;
        for(int i = 0 ; i < UBC_Rates.size() ; i++){
            if(UBC_Rates[i] > UBC_Rates[max_index]){
                max_index = i;
            }
        }

        //return the best move
        std::string move = std::to_string(valid_moves[max_index]);
        game->simulate(SIMULATE_STATE::LOAD_BOARD);
        return move;
    }
  
};
