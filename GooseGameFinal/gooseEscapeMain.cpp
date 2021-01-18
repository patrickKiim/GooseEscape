// gooseEscapeMain.cpp

#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console. Don't modify this line!
Console out;

int main()
{
    //Set up the window. Don't edit these two lines
    terminal_open();
    terminal_set(SETUP_MESSAGE);

    //seed rand() (adapted from previous c++ knowledge)
    srand(time(NULL));

    // make the player. Spawns at very right-edge with randomized y-position.
    Actor player(PLAYER_CHAR, MAX_BOARD_X, rand() % MAX_BOARD_Y, PLAYER_LIFE);

    // make the monster
    Actor monster(MONSTER_CHAR,
                  rand() % (MAX_BOARD_X - MONSTER_MAX_RIGHT) + MONSTER_MAX_LEFT,
                  rand() % MAX_BOARD_Y);

    // make moving cars on two column of roads
    Actor road1[NUM_CARS];
    Actor road2[NUM_CARS];
    int road1Position = (rand() % ROAD_RANDOM_FACTOR) + ROAD_1_POSITION;
    int road2Position = (rand() % ROAD_RANDOM_FACTOR) + ROAD_2_POSITION;
    for (int index = 0; index < NUM_CARS; index++)
    {
        road1[index].change_char(CAR_CHAR);
        road1[index].update_location(road1Position, CAR_DISTANCE * index + CAR_OFFSET_1);
        road2[index].change_char(CAR_CHAR);
        road2[index].update_location(road2Position, CAR_DISTANCE * index + CAR_OFFSET_2);
    }

    // Declare the array that will hold the game board "map"
    int gameMap[NUM_BOARD_X][NUM_BOARD_Y] = {0};

    // Place safe zone. Spawns at very left-edge with randomized y-position.
    int win_location_y = rand() % MAX_BOARD_Y;
    gameMap[MIN_BOARD_X][win_location_y] = WINNER;

    //initialize location for walls.
    for (int index = WALL1_TOP; index <= WALL1_BOTTOM; index++)
        gameMap[WALL1_COLUMN][index] = SHALL_NOT_PASS;

    for (int index = WALL2_TOP; index <= WALL2_BOTTOM; index++)
        gameMap[WALL2_COLUMN][index] = SHALL_NOT_PASS;

    for (int index = WALL3_TOP; index <= WALL3_BOTTOM; index++)
        gameMap[WALL3_COLUMN][index] = SHALL_NOT_PASS;

    // Set up Goose Nest
    int goose_nest_x = GOOSE_NEST_X + rand() % NEST_LOCATION_RANDOM_FACTOR;
    int goose_nest_y = GOOSE_NEST_Y + rand() % NEST_LOCATION_RANDOM_FACTOR;
    gameMap[goose_nest_x][goose_nest_y] = GOOSE_NEST;
    Actor motherGoose(MOTHER_GOOSE_CHAR, goose_nest_x + 1, goose_nest_y + 1);

    // Place a collectable key right under the goose nest
    int key_x = goose_nest_x - 1;
    int key_y = goose_nest_y - 1;
    gameMap[key_x][key_y] = KEY;

    // true when the user obtains the Key
    bool keyGet = false;

    // Call the function to print the game board
    printGameBoard(gameMap);

    // Printing the instructions
    out.writeLine("Escape the Goose (G)! The goose can sometimes jump over walls (o)!");
    out.writeLine("Use the arrow keys to move (%). Don't get hit by cars (U)!");
    out.writeLine("Beware of the Mama Goose (M). She is near her nest (#) at all times!");
    out.writeLine("Current Life: " + player.get_life_string());

    // Timer that controls speed of actors that move by themselves:
    clock_t startTime = clock(), countTime = clock();

/*
    This is the main game loop.  It continues to let the player give input
    as long as they do not press escape or close, they are not captured by
    the goose, hit by a car, or did not reach the win tile
*/
    int keyEntered = TK_A;  // can be any valid value that is not ESC or CLOSE

    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE
          && !captured(player, monster, goose_nest_x, goose_nest_y)
          && !captured(player, motherGoose, goose_nest_x, goose_nest_y)
          && !hitByCar(player, road1) && !hitByCar(player, road2)
          && !hasWon(player, MIN_BOARD_X, win_location_y, keyGet))
    {
        // get player key press
        keyEntered = terminal_read();

        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player
            movePlayer(keyEntered, player, gameMap);
            // check if player is on key tile, then get key if necessary.
            getKey(player, key_x, key_y, gameMap, keyGet);
        }

        // This code runs if a certain time interval has passed:
        countTime = clock();
        if (countTime - startTime >= GAME_SPEED)
        {
            // call the goose's chase function
            moveMonster(player, monster, gameMap);
            // call mother goose's chase function
            moveMotherGoose(player, motherGoose, gameMap, goose_nest_x, goose_nest_y);
            // move cars
            moveCars(road1, road2);
            // reset timer
            countTime = clock();
            startTime = clock();
        }
        printGameBoard(gameMap);
    }

    if (keyEntered != TK_CLOSE)
    {
        //once we're out of the loop, the game is over
        out.writeLine("");
        out.writeLine("");
        out.writeLine("Game has ended");
        // output why: did the goose get us, or did we get to the win location?
        if (captured(player,monster, goose_nest_x, goose_nest_y))
            out.writeLine("The goose has caught you!");
        else if (hitByCar(player, road1) || hitByCar(player, road2))
            out.writeLine("Watch your step! You got hit by a car!");
        else if (hasWon(player, MIN_BOARD_X, win_location_y, keyGet))
            out.writeLine("You have arrived at the safe zone!");

        // Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

    //game is done, close it
    terminal_close();
}
