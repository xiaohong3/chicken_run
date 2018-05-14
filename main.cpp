#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "Enemy.h"

using namespace std;

bool init();

void render();
SDL_Texture* load_texture(const string &str);
void run_game();

void add_enemy();
void move_enemies();

void reset_player_pos();
void setup_start();

bool check_collision(const SDL_Rect& rect1, const SDL_Rect& rect2);
bool check_enemy_collisions();
bool check_result();

const int WIN_W = 300;
const int WIND_H = 400;

const int ENEMY_AMOUNT = 13;
const int ENEMY_SIZE = 20;
const int PLAYER_SIZE = 20;
const int ENEMY_SPEED = 1;
const int PLAYER_SPEED = 15;
const int ENEMY_MOVE_UP_FACTOR = 25;

const int BAR_H = 20;

int last_enemy_pos = 50;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Rect player;
SDL_Rect top_bar;
SDL_Rect bottom_bar;
SDL_Rect background;
std::vector<Enemy> enemies;

SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* enemyTexture = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* barTexture = nullptr;

int main()
{
    if (!init())
    {
        return -1;
    }

    setup_start();
    // reset_player_pos();
    run_game();

    return 0;
}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        window = SDL_CreateWindow("Chicken Run", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_W, WIND_H, 0);

        if( window == nullptr )
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, 0);

            if (renderer == nullptr)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
        }
    }

    return success;
}

void setup_start()
{
    background.x = 0;
    background.y = 0;
    background.w = WIN_W;
    background.h = WIND_H;

    backgroundTexture = load_texture("resources/background.bmp");
    barTexture = load_texture("resources/bar.bmp");
    enemyTexture = load_texture("resources/enemy.bmp");
    playerTexture = load_texture("resources/player.bmp");

    for (int i = 0; i < ENEMY_AMOUNT; ++i)
    {
        add_enemy();
    }

    top_bar.x = 0;
    top_bar.y = 0;
    top_bar.w = WIN_W;
    top_bar.h = BAR_H;

    bottom_bar.x = 0;
    bottom_bar.y = WIND_H - BAR_H;
    bottom_bar.w = WIN_W;
    bottom_bar.h = BAR_H;

    reset_player_pos();

    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;

}

void reset_player_pos()
{
    player.x = WIN_W / 2 - PLAYER_SIZE / 2;
    player.y = WIND_H - BAR_H;
}

void run_game()
{
    bool quit = false;

    while (!quit)
    {
        SDL_Event e;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        player.y -= PLAYER_SPEED;
                        break;

                    case SDLK_DOWN:
                        player.y += PLAYER_SPEED;
                        break;

                    case SDLK_LEFT:
                        player.x -= PLAYER_SPEED;
                        break;

                    case SDLK_RIGHT:
                        player.x += PLAYER_SPEED;
                        break;

                    default:
                        break;
                }
            }
        }

        move_enemies();

        if (check_enemy_collisions() == true)
        {
            // Could end the game and say lose
            reset_player_pos();
        }

        if (player.y < (top_bar.y + top_bar.h))
        {
            // Could end the game and say win
            reset_player_pos();
        }

        // check_result();

        render();

        // Add a 16msec delay to make our game run at ~60 fps
        SDL_Delay(16);
    }
}

void add_enemy()
{
    if ((rand() % 2) == 0)
    {
        enemies.push_back(Enemy({rand() % WIN_W, last_enemy_pos, ENEMY_SIZE, ENEMY_SIZE}, ENEMY_SPEED, Direction::Left));
    }
    else
    {
        enemies.push_back(Enemy({rand() % WIN_W, last_enemy_pos, ENEMY_SIZE, ENEMY_SIZE}, ENEMY_SPEED, Direction::Right));
    }
    last_enemy_pos += ENEMY_MOVE_UP_FACTOR;
}

void move_enemies()
{
    for (Enemy& p : enemies)
    {
        if (p.dir == Direction::Left)
        {
            p.pos.x -= ENEMY_SPEED;

            if ((p.pos.x + p.pos.w) <= 0)
            {
                p.pos.x = WIN_W - p.pos.w;
            }
        }
        else
        {
            p.pos.x += ENEMY_SPEED;

            if ((p.pos.x >= WIN_W))
            {
                p.pos.x = 0;
            }
        }
    }
}

bool check_enemy_collisions()
{
    for (Enemy& p : enemies)
    {
        if (check_collision(p.pos, player))
        {
            return true;
        }
    }

    return false;
}

bool check_collision(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
    int rect1_left = rect1.x;
    int rect1_right = rect1.x + rect1.w;
    int rect1_top = rect1.y;
    int rect1_bottom = rect1.y + rect1.h;

    int rect2_left = rect2.x;
    int rect2_right = rect2.x + rect2.w;
    int rect2_top = rect2.y;
    int rect2_bottom = rect2.y + rect2.h;

    if (rect1_left > rect2_right)
    {
        return false;
    }

    if (rect1_right < rect2_left)
    {
        return false;
    }

    if (rect1_top > rect2_bottom)
    {
        return false;
    }

    if (rect1_bottom < rect2_top)
    {
        return false;
    }

    return true;
}

SDL_Texture* load_texture(const string& str)
{
    SDL_Surface* surface = SDL_LoadBMP(str.c_str());

    if (surface == nullptr)
    {
        printf("SDL surface could not be loaded! SDL Error: %s\n", SDL_GetError());
        return nullptr;
    }
    else
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (texture == nullptr)
        {
            printf("SDL texture could not be created! SDL Error: %s\n", SDL_GetError());
            return nullptr;
        }
        else
        {
            return texture;

        }
    }
}

void render()
{
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, backgroundTexture, NULL, &background);
    SDL_RenderCopy(renderer, barTexture, NULL, &top_bar);
    SDL_RenderCopy(renderer, barTexture, NULL, &bottom_bar);

	SDL_RenderCopy(renderer, playerTexture, NULL, &player);

	for (const Enemy& p : enemies)
	{
        SDL_RenderCopy(renderer, enemyTexture, NULL, &p.pos);
	}

	SDL_RenderPresent(renderer);
}



