//gooseEscapeGamePlay.cpp
#include <iostream>
#include <cmath>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

extern Console out;
/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner.  So it means the coordinate axes are:
---------------->   x direction
|
|
|
|
|
V

y direction
*/

/*
    Print the game world

    The functions draws characters to present features of the game
    board, e.g. win location, walls, goose nests
*/
void printGameBoard(int gameMap[NUM_BOARD_X][NUM_BOARD_Y])
{
    for (int location_x = 0; location_x < NUM_BOARD_X; location_x++)
    {
        for (int location_y = 0; location_y < NUM_BOARD_Y; location_y++)
        {
            if (gameMap[location_x][location_y] == SHALL_NOT_PASS)
                terminal_put(location_x, location_y, WALL_CHAR);
            else if (gameMap[location_x][location_y] == WINNER)
                terminal_put(location_x, location_y, WIN_CHAR);
            else if (gameMap[location_x][location_y] == GOOSE_NEST)
                terminal_put(location_x, location_y, GOOSE_NEST_CHAR);
            else if (gameMap[location_x][location_y] == KEY)
                terminal_put(location_x, location_y, KEY_CHAR);
        }
    }
    terminal_refresh();
}

/*
    Returns true if the goose and the player collide (lose condition)
*/
bool captured(Actor & player, Actor & monster, int goose_nest_x, int goose_nest_y)
{
    if (player.isOverlapping(monster))
    {
        player.decrease_life();
        if (player.get_life() <= 0)
        {
            return true;
        }
        else
        {
            out.writeLine("Ouch! You have lost a life!");
            out.writeLine("Current Life: " + player.get_life_string());
            monster.set_location(goose_nest_x, goose_nest_y);
        }
    }
    return false;
}

/*
    Returns true if a car and player collide (lose condition)
*/
bool hitByCar(Actor & player, Actor cars[])
{
    for (int index = 0; index < NUM_CARS; index++)
    {
        if (player.isOverlapping(cars[index]))
        {
            player.decrease_life();
            if (player.get_life() <= 0)
            {
                return true;
            }
            else
            {
                out.writeLine("Ouch! You have lost a life!");
                out.writeLine("Current Life: " + player.get_life_string());
            }
        }
    }
    return false;
}

/*
    if the player actor is overlapping with the key,
    turns bool keyGet to true and removes the key from the gamemap.
*/
void getKey(Actor & player, int key_x, int key_y,
            int gameMap[NUM_BOARD_X][NUM_BOARD_Y], bool & keyGet)
{
    if (player.get_x() == key_x && player.get_y() == key_y)
    {
        gameMap[key_x][key_y] = EMPTY;
        keyGet = true;
        out.writeLine("You have collected the Key!");
    }
}

/*
    Returns true if the player is on the safe zone (win condition)
*/
bool hasWon(Actor const & player, int const win_location_x,
            int const win_location_y, bool keyGet)
{
    if (player.get_x() == win_location_x && player.get_y() == win_location_y)
    {
        if (keyGet)
            return true;
        else
        {
            out.writeLine("The doors are locked! You must collect the Key!");
            return false;
        }
    }
    return false;
}

/*
    Move the player to a nearest adjacent tile based on the user input
    if the move is valid:
    Up Arrow Key: move 1 tile up
    Down Arrow Key: move 1 tile down
    Left Arrow Key: move 1 tile left
    Right Arrow Key: move 1 tile right
*/
void movePlayer(int key, Actor & player, int gameMap[NUM_BOARD_X][NUM_BOARD_Y])
{
    int yMove = 0, xMove = 0;
    if (key == TK_UP)
        yMove = -1;
    else if (key == TK_DOWN)
        yMove = 1;
    else if (key == TK_LEFT)
        xMove = -1;
    else if (key == TK_RIGHT)
        xMove = 1;

    if (player.no_wall_in_path(xMove, yMove, gameMap))
        player.update_location(xMove, yMove);
}

/*
    moves monster one tile closer to the the player
*/
void moveMonster(Actor & player, Actor & monster,
                 int gameMap[NUM_BOARD_X][NUM_BOARD_Y])
{
    int delX = player.get_x() - monster.get_x();
    int delY = player.get_y() - monster.get_y();

    //allows movement in 1 space only
    int xMove = delX / fabs(delX);
    int yMove = delY / fabs(delY);

    //bound checking and moving is split for x direction and y direction
    //allows movement in 1 diretion if can move.
    if (rand() % JUMP_PROBABILITY_DENOMINATOR < JUMP_PROBABILITY_NUMERATOR)
    {
        monster.update_location(xMove, yMove);
    }
    else
    {
        if (monster.no_wall_in_path(xMove, 0, gameMap)
            && monster.no_safespace_in_path(xMove,0, gameMap))
            monster.update_location(xMove, 0);
        if (monster.no_wall_in_path(0, yMove, gameMap)
            && monster.no_safespace_in_path(xMove,0, gameMap))
            monster.update_location(0, yMove);
    }

}

/*
    moves mother goose. Has an algorithm to stay near the next unless the
    player comes too close to it.
*/
void moveMotherGoose(Actor & player, Actor & monster,
                     int gameMap[NUM_BOARD_X][NUM_BOARD_Y], int goose_nest_x,
                     int goose_nest_y)
{
    int delX_player = player.get_x() - monster.get_x();
    int delY_player = player.get_y() - monster.get_y();
    int delX_nest = player.get_x() - goose_nest_x;
    int delY_nest = player.get_y() - goose_nest_y;

    if ((fabs(delX_player) < CHASE_PROX_GOOSE
         && fabs(delY_player) < CHASE_PROX_GOOSE)
        || (fabs(delX_nest) < CHASE_PROX_NEST
            && fabs(delY_nest) < CHASE_PROX_NEST))
    {
        moveMonster(player, monster, gameMap);
    }
    else
    {
        // allows movement in 1 space only, always stay close to nest.
        int xMove = (rand() % MOVE_RAND_FACTOR) - INDEX_SHIFT;
        int yMove = (rand() % MOVE_RAND_FACTOR) - INDEX_SHIFT;
        if (monster.get_x() < goose_nest_x - STAY_NEAR_NEST_PROX)
            xMove = 1;
        else if (monster.get_x() > goose_nest_x + STAY_NEAR_NEST_PROX)
            xMove = -1;
        if (monster.get_y() < goose_nest_x - STAY_NEAR_NEST_PROX)
            yMove = 1;
        else if (monster.get_y() > goose_nest_y + STAY_NEAR_NEST_PROX)
            yMove = -1;
        monster.update_location(xMove, yMove);
    }
}

/*
    moves all cars 1 tile up. If car reaches MIN_BOARD_Y, reset to MAX_BOARD_Y
*/
void moveCars(Actor road1[], Actor road2[])
{
    for (int index = 0; index < NUM_CARS; index++)
    {
        if (road1[index].get_y() <= MIN_BOARD_Y)
            road1[index].update_location(0, MAX_BOARD_Y);
        else
            road1[index].update_location(0, -1);
        if (road2[index].get_y() <= MIN_BOARD_Y)
            road2[index].update_location(0, MAX_BOARD_Y);
        else
            road2[index].update_location(0, -1);
    }
}
