#include <malloc.h>
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TITLE "Empire"
#define WIDTH 800
#define HEIGHT 600

#define FPS_CAP 30
#define FRAME_DELAY (1000 / FPS_CAP)

struct cell
{
    bool alive;
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT);

    unsigned int *pixels = malloc(WIDTH * HEIGHT * sizeof(unsigned int));
    memset(pixels, 255, WIDTH * HEIGHT * sizeof(unsigned int));

    srand((unsigned int)time(NULL));

    bool step = true;

    struct cell *cells = malloc(WIDTH * HEIGHT * sizeof(struct cell));

start:
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            struct cell *cell = &cells[x + y * WIDTH];

            cell->alive = rand() % 2 == 0;
        }
    }

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
        }

        for (int x = 0; x < WIDTH; x++)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                unsigned int *pixel = &pixels[x + y * WIDTH];
                struct cell *cell = &cells[x + y * WIDTH];

                int red = (*pixel >> 24) & 0xff;
                int green = (*pixel >> 16) & 0xff;
                int blue = (*pixel >> 8) & 0xff;
                int alpha = (*pixel >> 0) & 0xff;

                if (cell->alive)
                {
                    red = 255;
                    green = 255;
                    blue = 255;
                }
                else
                {
                    red = 0;
                    green = 0;
                    blue = 0;
                }

                *pixel = ((red & 0xff) << 24) | ((green & 0xff) << 16) | ((blue & 0xff) << 8) | ((alpha & 0xff) << 0);
            }
        }

        SDL_UpdateTexture(
            texture,
            NULL,
            pixels,
            WIDTH * sizeof(unsigned int));

        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;

        if (FRAME_DELAY > frame_time)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    free(cells);

    free(pixels);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
