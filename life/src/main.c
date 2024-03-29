#include <malloc.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_TITLE "Conway's Life"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

struct cell
{
    bool alive;
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
    struct cell *new_cells = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));

start:
    for (int x = 0; x < WINDOW_WIDTH; x++)
    {
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            struct cell *cell = &cells[x + y * WINDOW_WIDTH];

            cell->alive = rand() % 20 == 0;
        }
    }

    bool running = true;
    while (running)
    {
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
            memcpy(new_cells, cells, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));

            for (int x = 0; x < WINDOW_WIDTH; x++)
            {
                for (int y = 0; y < WINDOW_HEIGHT; y++)
                {
                    struct cell *cell = &cells[x + y * WINDOW_WIDTH];
                    struct cell *new_cell = &new_cells[x + y * WINDOW_WIDTH];

                    int live_neighbors = 0;

                    for (int dx = -1; dx <= 1; dx++)
                    {
                        for (int dy = -1; dy <= 1; dy++)
                        {
                            if (dx == 0 && dy == 0)
                            {
                                continue;
                            }

                            int nx = x + dx;
                            int ny = y + dy;

                            if (nx < 0)
                            {
                                nx = WINDOW_WIDTH + nx;
                            }

                            if (nx >= WINDOW_WIDTH)
                            {
                                nx = WINDOW_WIDTH - nx;
                            }

                            if (ny < 0)
                            {
                                ny = WINDOW_HEIGHT + ny;
                            }

                            if (ny >= WINDOW_HEIGHT)
                            {
                                ny = WINDOW_HEIGHT - ny;
                            }

                            struct cell *neighbor = &cells[nx + ny * WINDOW_WIDTH];

                            if (neighbor->alive)
                            {
                                live_neighbors++;
                            }
                        }
                    }

                    if (cell->alive && live_neighbors != 2 && live_neighbors != 3)
                    {
                        new_cell->alive = false;
                    }
                    else if (live_neighbors == 3)
                    {
                        new_cell->alive = true;
                    }
                }
            }

            memcpy(cells, new_cells, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));
        }

        int num_live = 0;
        int num_dead = 0;

        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            for (int y = 0; y < WINDOW_HEIGHT; y++)
            {
                unsigned int *pixel = &pixels[x + y * WINDOW_WIDTH];
                struct cell *cell = &cells[x + y * WINDOW_WIDTH];

                int red = (*pixel >> 24) & 0xff;
                int green = (*pixel >> 16) & 0xff;
                int blue = (*pixel >> 8) & 0xff;
                int alpha = (*pixel >> 0) & 0xff;

                if (cell->alive)
                {
                    red = 255;
                    green = 255;
                    blue = 255;

                    num_live++;
                }
                else
                {
                    red = 0;
                    green = 0;
                    blue = 0;

                    num_dead++;
                }

                *pixel = ((red & 0xff) << 24) | ((green & 0xff) << 16) | ((blue & 0xff) << 8) | ((alpha & 0xff) << 0);
            }
        }

        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "%s - Live: %d, Dead: %d", WINDOW_TITLE, num_live, num_dead);
        SDL_SetWindowTitle(window, buffer);

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(
            screen,
            NULL,
            pixels,
            WINDOW_WIDTH * sizeof(unsigned int));
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    free(cells);
    free(new_cells);

    free(pixels);

    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
