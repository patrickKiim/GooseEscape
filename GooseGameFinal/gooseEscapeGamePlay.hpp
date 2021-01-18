//gooseEscapeGamePlay.hpp
#ifndef GOOSE_ESCAPE_GAMEPLAY
#define GOOSE_ESCAPE_GAMEPLAY
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"

/*
    This file is all about the game world.  You will modify this to add
    constants and function prototypes.  Modify gooseGamePlay.cpp to
	actually add functionality.
*/

// Constants are declared below:

/*
    Beginning Player Life
*/
const int PLAYER_LIFE = 3;

/*
    How fast elements such as monster and cars move in milliseconds
*/
const int GAME_SPEED = 150;

/*
    characters displayed on the game console coresponding to actor
*/
const int PLAYER_CHAR = int('@');
const int MONSTER_CHAR = int('G');
const int MOTHER_GOOSE_CHAR = int('M');
const int WALL_CHAR = int('o');
const int WIN_CHAR = int('%');
const int CAR_CHAR = int('U');
const int GOOSE_NEST_CHAR = int('#');
const int KEY_CHAR = int('Q');

/*
    Monster (Goose) properties
*/
const int MONSTER_MAX_LEFT = 10;
const int MONSTER_MAX_RIGHT = 30;
const int JUMP_PROBABILITY_NUMERATOR = 1;
const int JUMP_PROBABILITY_DENOMINATOR = 5;

/*
    Mama goose properties
*/
const int GOOSE_NEST_X = 10;
const int GOOSE_NEST_Y = 8;
const int NEST_LOCATION_RANDOM_FACTOR = 6;
const int STAY_NEAR_NEST_PROX = 3;
const int CHASE_PROX_GOOSE = 7;
const int CHASE_PROX_NEST = 10;
const int INDEX_SHIFT = 1;
const int MOVE_RAND_FACTOR = 3;

/*
    Car properties
*/
const int NUM_ROADS = 3;
const int ROAD_RANDOM_FACTOR = 10;
const int ROAD_1_POSITION = 20;
const int ROAD_2_POSITION = 50;
const int NUM_CARS = 6;
const int CAR_DISTANCE = 2;
const int CAR_OFFSET_1 = 1;
const int CAR_OFFSET_2 = 7;

/*
    wall locations
*/
const int WALL1_COLUMN = 4;
const int WALL1_TOP = 3;
const int WALL1_BOTTOM = 15;
const int WALL2_COLUMN = 45;
const int WALL2_TOP = 10;
const int WALL2_BOTTOM = 20;
const int WALL3_COLUMN = 35;
const int WALL3_TOP = 0;
const int WALL3_BOTTOM = 10;


// Game play function prototypes are give below:

/*
    Print the game world.

    The functions draws characters to present features of the game
    board, e.g. win location, obstacles, power ups
*/
void printGameBoard(int gameMap[NUM_BOARD_X][NUM_BOARD_Y]);

/*
    Returns true if the goose and the player collide (lose condition)
*/
bool captured(Actor & player, Actor & monster, int goose_nest_x, int goose_nest_y);

/*
    Returns true if a car and player collide (lose condition)
*/
bool hitByCar(Actor & player, Actor cars[]);

/*
    Turns keyGet to true and removes the key
    if the player is overlapping with the key
*/
void getKey(Actor & player, int key_x, int key_y,
            int gameMap[NUM_BOARD_X][NUM_BOARD_Y], bool & keyGet);

/*
    Returns true if the player is on the safe zone (win condition)
*/
bool hasWon(Actor const & player, int const win_location_x,
            int const win_location_y, bool keyGet);

/*
    Checks if an actor collides with a SHALL_NOT_PASS tile, given the
    change in x and y from current position (int delta_x, int delta_y)
*/
bool collideWall(Actor actor, int delta_x, int delta_y,
                 int gameMap[NUM_BOARD_X][NUM_BOARD_Y]);

/*
    Move the player to a nearest adjacent tile based on the user input
    if the move is valid:
    Up Arrow Key: move 1 tile up
    Down Arrow Key: move 1 tile down
    Left Arrow Key: move 1 tile left
    Right Arrow Key: move 1 tile right
*/
void movePlayer(int key, Actor & player,
                int gameMap[NUM_BOARD_X][NUM_BOARD_Y]);

/*
    moves monster one tile closer to the the player.
*/
void moveMonster(Actor & player, Actor & monster,
                 int gameMap[NUM_BOARD_X][NUM_BOARD_Y]);

/*
    moves mother goose. Has an algorithm to stay near the next unless the
    player comes too close to it.
*/
void moveMotherGoose(Actor & player, Actor & monster,
                     int gameMap[NUM_BOARD_X][NUM_BOARD_Y], int goose_nest_x,
                     int goose_nest_y);

/*
    moves all cars 1 tile up. If car reaches MIN_BOARD_Y, reset to MAX_BOARD_Y
*/
void moveCars(Actor road1[], Actor road2[]);


#endif