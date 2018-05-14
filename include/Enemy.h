#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>

enum class Direction
{
    Left, Right
};

struct Enemy
{
    Enemy(SDL_Rect pos_, int speed_, Direction dir_)
    {
        pos = pos_;
        speed = speed_;
        dir = dir_;
    }

    SDL_Rect pos;
    int speed;
    Direction dir;
};

#endif // ENEMY_H
