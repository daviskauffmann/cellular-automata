#include <malloc.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_TITLE "Langton's Ant"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS_CAP 30
#define FRAME_DELAY (1000 / FPS_CAP)

struct cell
{
    unsigned int index;
};

enum direction
{
    DIRECTION_EAST,
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_WEST
};

struct ant
{
    int x;
    int y;
    enum direction direction;
};

struct rule
{
    unsigned char red;
    unsigned char blue;
    unsigned char green;
    bool right;
};

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_SOFTWARE);

    SDL_Texture *screen = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    unsigned int *pixels = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(unsigned int));

    srand((unsigned int)time(NULL));

    bool step = true;

    struct cell *cells = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));
    struct ant *ant = malloc(sizeof(struct ant));
    struct rule rules[] = {
        {0, 0, 0, false},
        {255, 255, 255, true}};

start:
    for (int x = 0; x < WINDOW_WIDTH; x++)
    {
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            struct cell *cell = &cells[x + y * WINDOW_WIDTH];

            cell->index = 0;
        }
    }

    ant->x = WINDOW_WIDTH / 2;
    ant->y = WINDOW_HEIGHT / 2;
    ant->direction = DIRECTION_NORTH;

    bool running = true;
    while (running)
    {
        unsigned int frame_start = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                {
                    running = false;
                }
                break;
                case SDLK_SPACE:
                {
                    step = !step;
                }
                break;
                case SDLK_r:
                {
                    goto start;
                }
                break;
                }
            }
            break;
            case SDL_QUIT:
            {
                running = false;
            }
            break;
            }
        }

        if (step)
        {
            if (ant->x < 0)
            {
                ant->x = WINDOW_WIDTH + ant->x;
            }

            if (ant->x >= WINDOW_WIDTH)
            {
                ant->x = WINDOW_WIDTH - ant->x;
            }

            if (ant->y < 0)
            {
                ant->y = WINDOW_HEIGHT + ant->y;
            }

            if (ant->y >= WINDOW_HEIGHT)
            {
                ant->y = WINDOW_HEIGHT - ant->y;
            }

            struct cell *cell = &cells[ant->x + ant->y * WINDOW_WIDTH];
            struct rule *rule = &rules[cell->index];

            switch (ant->direction)
            {
            case DIRECTION_EAST:
            {
                if (rule->right)
                {
                    ant->direction = DIRECTION_SOUTH;
                    ant->y++;
                }
                else
                {
                    ant->direction = DIRECTION_NORTH;
                    ant->y--;
                }
            }
            break;
            case DIRECTION_NORTH:
            {
                if (rule->right)
                {
                    ant->direction = DIRECTION_EAST;
                    ant->x++;
                }
                else
                {
                    ant->direction = DIRECTION_WEST;
                    ant->x--;
                }
            }
            break;
            case DIRECTION_SOUTH:
            {
                if (rule->right)
                {
                    ant->direction = DIRECTION_WEST;
                    ant->x--;
                }
                else
                {
                    ant->direction = DIRECTION_EAST;
                    ant->x++;
                }
            }
            break;
            case DIRECTION_WEST:
            {
                if (rule->right)
                {
                    ant->direction = DIRECTION_NORTH;
                    ant->y--;
                }
                else
                {
                    ant->direction = DIRECTION_SOUTH;
                    ant->y++;
                }
            }
            break;
            }

            if (++cell->index >= sizeof(rules) / sizeof(struct rule))
            {
                cell->index = 0;
            }
        }

        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            for (int y = 0; y < WINDOW_HEIGHT; y++)
            {
                unsigned int *pixel = &pixels[x + y * WINDOW_WIDTH];
                struct cell *cell = &cells[x + y * WINDOW_WIDTH];
                struct rule *rule = &rules[cell->index];

                int red = (*pixel >> 24) & 0xff;
                int green = (*pixel >> 16) & 0xff;
                int blue = (*pixel >> 8) & 0xff;
                int alpha = (*pixel >> 0) & 0xff;

                if (x == ant->x && y == ant->y)
                {
                    red = 255;
                    green = 0;
                    blue = 0;
                }
                else
                {
                    red = rule->red;
                    green = rule->green;
                    blue = rule->blue;
                }

                *pixel = ((red & 0xff) << 24) | ((green & 0xff) << 16) | ((blue & 0xff) << 8) | ((alpha & 0xff) << 0);
            }
        }

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(
            screen,
            NULL,
            pixels,
            WINDOW_WIDTH * sizeof(unsigned int));
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;

        if (FRAME_DELAY > frame_time)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    free(ant);
    free(cells);

    free(pixels);

    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
