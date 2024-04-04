
#pragma once // meaning include this file only once
#include "../MySTL/vector.h"
#include "../Engine/GameBoard.h"
#include <iostream>
#include <string>

class TTTBoard : public Board<3, 3, int>
{
private:
public:
    void init();
    void display();

    template <typename U>
    bool playable(U loc);

    template <typename U>
    U check_terminal();

    template <typename U, typename V>
    void move(U loc, V turn);

    template <typename U>
    Vector<U> get_valid_moves();
};

void TTTBoard::init()
{
    Board::init();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            game_board[i][j] = -1;
        }
    }
}

void TTTBoard::display()
{
    for (auto row : TTTBoard::game_board)
    {
        cout << "| ";
        for (auto col : row)
        {
            if (col == -1)
                cout << "  | ";
            else if (col == 0)
                cout << "O | ";
            else if (col == 1)
                cout << "X | ";
        }
        cout << endl;
    }
}

template <>
void TTTBoard::move<int, int>(int loc, int turn)
{
    int row = (loc - 1) / 3;
    int col = (loc - 1) % 3;

    if (game_board[row][col] != -1)
    {
        cout << "Invalid Move" << endl;
        // Sometype of error handling
        // finished it cuntt....
        // okie
    }
    else
    {
        game_board[row][col] = turn;
    }
}

// i swear to god i'll .... , whyyyyyy
// check the typeee you fooollll
// Ah i see mb

template <>
int TTTBoard::check_terminal<int>()
{
    // cout<<"Checking for terminal"<<endl;
    // return 0 -> player 1 , 1->player2 , 2-> draw , -1 -> game not over

    // make enum for these values
    // Game engine must me able to decode it somehow

    // check for a draw
    if (get_remaining_moves() == 0)
    {
        return -2;
    }

    // Check for rows
    for (int i = 0; i < 3; i++)
    {
        if (game_board[i][0] != -1 && game_board[i][0] == game_board[i][1] && game_board[i][1] == game_board[i][2])
        {
            return game_board[i][0];
        }
    }

    // Check for columns
    for (int i = 0; i < 3; i++)
    {
        if (game_board[i][0] != -1 && game_board[0][i] == game_board[1][i] && game_board[1][i] == game_board[2][i])
        {
            return game_board[0][i];
        }
    }

    // Check for diagonals
    if ((game_board[1][1] != -1) && ((game_board[0][0] == game_board[1][1] && game_board[1][1] == game_board[2][2]) || (game_board[0][2] == game_board[1][1] && game_board[1][1] == game_board[2][0])))
    {
        return game_board[1][1];
    }

    return -1;
}

template <>
bool TTTBoard::playable<int>(int loc)
{
    int row = (loc - 1) / 3;
    int col = (loc - 1) % 3;
    return game_board[row][col] == -1;
}

template <>
Vector<int> TTTBoard::get_valid_moves<int>()
{
    Vector<int> valid_moves;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (game_board[i][j] == -1)
            {
                valid_moves.push_back(i * 3 + j + 1);
            }
        }
    }
    return valid_moves;
}
