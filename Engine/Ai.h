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
    GAME* game;
    double exploration_factor = 0.5;

public:
    
    AI(GAME* game) : game(game) {}

    void set_exploration_factor(double factor) {
        exploration_factor = factor;
    }

    
    std::string decide_move()
    {
        game->simulate(SIMULATE_STATE::SAVE_BOARD);
        Vector<typename GAME::MOVE> valid_moves = game->get_valid_moves();
        Vector<double> UBC_Rates;

        typename GAME::PLAYER_NOTATION ai_notation = game->get_turn();
        int num_players = game->get_num_players();

        for(auto moves : valid_moves){

            Vector<int> result = simulate_game<typename GAME::MOVE,typename GAME::PLAYER_NOTATION>(moves,1);
            int total_games = result[0] + result[1] + result[2];
            double UCB = ((result[0]*2.0 + result[1]*1.0 - result[2]*2.0)/total_games) + sqrt(2 * log(total_games) / (total_games * exploration_factor));
            cout<<"FOR MOVE " << moves << " UCB IS " << UCB << endl;
            UBC_Rates.push_back(UCB);
        }
        
        int max_index = 0 ;
        for(int i = 0 ; i < UBC_Rates.size() ; i++){
            if(UBC_Rates[i] > UBC_Rates[max_index]){
                max_index = i;
            }
        }

        std::string move = std::to_string(valid_moves[max_index]);
        game->simulate(SIMULATE_STATE::LOAD_BOARD);
        return move;
    }

    template<typename T,typename U>
    Vector<int> simulate_game(T move,U turn) {
        game->simulate(move,turn);
        Vector<int> result(3, 0);
        Vector<typename GAME::MOVE> valid_moves = game->get_valid_moves();
        int terminal_state = game->get_game_state();
        
        if(terminal_state != -1){
            if(terminal_state == 1){
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


        if(valid_moves.size() == 0){
            result[1] = 1;
            game->simulate(move,SIMULATE_STATE::UNMOVE);
            return result;
        }


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
};
