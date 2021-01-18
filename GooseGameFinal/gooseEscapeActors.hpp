//gooseEscapeActors.hpp
#ifndef GOOSE_ESCAPE_ACTORS
#define GOOSE_ESCAPE_ACTORS
#include <cmath>
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"

/*
    Actors represent the entities in the game that change coordinates.
*/
class Actor
{
private:
    int actorChar;      
    int location_x, location_y;
    int life;

public:
      
    /*
        Default Constructor: 'A' is used to represent the actor, and
                             is placed on coordinate (0, 0).
    */
    Actor()
    {
        actorChar = int('A');
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        put_actor();
    }
    
    /*
        Constructor:
            char initPlayerChar - char used to represent actor on console
            x0 - initial x coordinate of actor
            y0 - initial y coordinate of actor
    */
    Actor(char initPlayerChar, int x0, int y0)
    {
        change_char(initPlayerChar);
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        update_location(x0,y0);
    }

    /*
        Constructor:
            char initPlayerChar - char used to represent actor on console
            x0 - initial x coordinate of actor
            y0 - initial y coordinate of actor
            life - # of life of actor. Useful for player actors.
    */
    Actor(char initPlayerChar, int x0, int y0, int life0)
    {
        change_char(initPlayerChar);
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        update_location(x0,y0);
        life = life0;
    }
    
    /*
        Accessor: Return x coordinate of actor
    */
    int get_x() const
    {
        return location_x;
    }
    
    /*
        Accessor: Return y coordinate of actor
    */
    int get_y() const
    {
        return location_y;
    }

    /*
        Accessor: Return life of actor as an integer
    */
    int get_life() const
    {
        return life;
    }
    
    /*
        Accessor: Return life of actor as a string
    */
    string get_life_string() const
    {
        char buffer[80];
        itoa(life,buffer,10);
        return string(buffer);
    }
    
    /*
        returns current coordinates in the format (x,y)
    */
    string get_location_string() const
    {
        char buffer[80];
        itoa(location_x,buffer,10);
        string formatted_location = "(" + string(buffer) + ",";
        itoa(location_y,buffer,10);
        formatted_location += string(buffer) + ")";
        return formatted_location;
    }

    /*
        Mutator: decrements life value by one
    */
    void decrease_life()
    {
        life--;
    }
    
    /*
        Mutator: changes char used to represent actor on console
    */
    void change_char(char new_actor_char)
    {
        actorChar = min(int('~'),max(int(new_actor_char),int(' ')));
    }

    /*
        Returns true if given location has no wall
    */
    bool no_wall_in_path(int delta_x, int delta_y,
                         int gameMap[NUM_BOARD_X][NUM_BOARD_Y])
    {
        return gameMap[location_x + delta_x][location_y + delta_y]
                != SHALL_NOT_PASS;
	}

    /*
        Returns true if desired movement location has no safespace
    */
    bool no_safespace_in_path(int delta_x, int delta_y,
                              int gameMap[NUM_BOARD_X][NUM_BOARD_Y])
    {
    	return gameMap[location_x + delta_x][location_y + delta_y]
                != WINNER;
	}

    /*
        returns true if the coordinates given by the change in x (int delta_x)
        and change in y (int delta_y) are within the boundary of the board
    */
    bool can_move(int delta_x, int delta_y) const
    {
        int new_x = location_x + delta_x;
        int new_y = location_y + delta_y;

        return new_x >= MIN_BOARD_X && new_x <= MAX_BOARD_X
          && new_y >= MIN_BOARD_Y && new_y <= MAX_BOARD_Y;
    }
    
    /*
        Removes the character from the current coordinates
        And places it on a new set of coordinates. The coordinates given
        must be valid (within game window)
    */
    void set_location(int x0, int y0)
    {
        int delta_x = x0 - (*this).location_x;
        int delta_y = y0 - (*this).location_y;
        update_location(delta_x, delta_y);
	}
    
    /*
        Removes the character from the current coordinates
        And places it on a new set of coordinates based on the
        desired change in x (int delta_x) and change in y (int delta_y)
    */
    void update_location(int delta_x, int delta_y)
    {
        if (can_move(delta_x, delta_y))
        {
            terminal_clear_area(location_x, location_y, 1, 1);
            location_x += delta_x;
            location_y += delta_y;
            put_actor();
        }
    }

    /*
        returns true if two actors are at same space
    */
    bool isOverlapping(Actor const & other) const
    {
      return (*this).location_x == other.get_x()
             && (*this).location_y == other.get_y();
    }
    
    /*
        updates the console such that current coordinates are reflected.
        Use when updating location.
    */
    void put_actor() const
    {
        terminal_put(location_x, location_y, actorChar);
        terminal_refresh();
    }
    
};
#endif