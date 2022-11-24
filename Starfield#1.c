#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>

typedef enum {false, true} bool;

const int NUMBER_OF_STARS = 200;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int MAX_Z = 800;
const int MAX_SPEED = 30;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;

float speed = .2;
bool is_running = true;

struct Star
{
    int x, y, sx, sy, last_sx, last_sy;
    float z, d;
};

void handle_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            is_running = false;
            SDL_Quit();
            break;

        case SDL_MOUSEMOTION:
            speed += (float)event.motion.xrel / 50;
            if (speed < 0) speed = 0;
            else if (speed > MAX_SPEED) speed = MAX_SPEED;
            break;

        default:
            break;
        }
    }
}

/*
    midpoint circle algorithm
*/
void fill_circle(SDL_Renderer *renderer, int center_x, int center_y, int r)
{
    if (r < 0) return;

    int x = 0, y = r;
    int p = 1 - r;

    SDL_RenderDrawLine(renderer, center_x + x, center_y + y, center_x + x, center_y - y);
    SDL_RenderDrawLine(renderer, center_x + y, center_y + x, center_x + y, center_y - x);
    SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x - x, center_y + y);
    SDL_RenderDrawLine(renderer, center_x - y, center_y - x, center_x - y, center_y + x);
    
    while (y >= x)
    {    
        if (p < 0)
        {
            x++;
            p += 2 * x + 1;
        }
        else
        {
            x++;
            y--;
            p += -2 * y + 2 * x + 1;
        }

        SDL_RenderDrawLine(renderer, center_x + x, center_y + y, center_x + x, center_y - y);
        SDL_RenderDrawLine(renderer, center_x + y, center_y + x, center_x + y, center_y - x);
        SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x - x, center_y + y);
        SDL_RenderDrawLine(renderer, center_x - y, center_y - x, center_x - y, center_y + x);
    }
}

void render(SDL_Renderer *renderer, struct Star *stars)
{
    /*
        clears the background
    */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /*
        draws the stars
    */
    SDL_SetRenderDrawColor(renderer, 100, 0, 200, 255);
    for (int i = 0; i < NUMBER_OF_STARS; i++)
    {
        fill_circle(renderer, stars[i].sx, stars[i].sy, 5 * stars[i].d);
        SDL_RenderDrawLine(renderer, stars[i].sx, stars[i].sy, stars[i].last_sx, stars[i].last_sy);
    }

    SDL_RenderPresent(renderer);
}

void update(struct Star *stars)
{
    for (int i = 0; i < NUMBER_OF_STARS; i++)
    {
        stars[i].z += speed;

        stars[i].d = stars[i].z / MAX_Z; 

        stars[i].last_sx = stars[i].sx;
        stars[i].last_sy = stars[i].sy;

        stars[i].sx = CENTER_X + stars[i].x * (stars[i].d + .2);
        stars[i].sy = CENTER_Y + stars[i].y * (stars[i].d + .2);

        /*
            resets a star if it reached its final position
        */
        if (stars[i].z > MAX_Z || stars[i].sx > SCREEN_WIDTH || stars[i].sx < 0 || stars[i].sy > SCREEN_WIDTH || stars[i].sy < 0)
        {             
            stars[i].x = SCREEN_WIDTH * 2 - rand() % 3201;
            stars[i].y = SCREEN_HEIGHT * 2 - rand() % 3201;
            stars[i].z = 0;

            stars[i].d = -1;

            stars[i].sx = CENTER_X + stars[i].x * .2;
            stars[i].sy = CENTER_Y + stars[i].y * .2;

            stars[i].last_sx = stars[i].sx;
            stars[i].last_sy = stars[i].sy;
        }
    }
}

int main(int argc, char const *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("starfield", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    struct Star stars[NUMBER_OF_STARS];
    for (int i = 0; i < NUMBER_OF_STARS; i++)
    {
        stars[i].x = SCREEN_WIDTH * 2 - rand() % 3201;
        stars[i].y = SCREEN_HEIGHT * 2 - rand() % 3201;
        stars[i].z = rand() % 801;

        stars[i].sx = CENTER_X + stars[i].x * (stars[i].z / MAX_Z);
        stars[i].sy = CENTER_Y + stars[i].y * (stars[i].z / MAX_Z);
    }

    /*
        mainloop
    */
    while (is_running)
    {
        handle_events();
        update(stars);
        render(renderer, stars);
    }

    return 0;
}
