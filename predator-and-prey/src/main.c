#include <malloc.h>
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TITLE "Predator & Prey"
#define WIDTH 800
#define HEIGHT 600

#define FPS_CAP 30
#define FRAME_DELAY (1000 / FPS_CAP)

#define HEALTH_START 100
#define HEALTH_REPRODUCE 200

enum type
{
    TYPE_EMPTY,
    TYPE_PREY,
    TYPE_PREDATOR
};

struct cell
{
    enum type type;
    int health;
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

            if (rand() % 2 == 0)
            {
                cell->type = TYPE_EMPTY;
                cell->health = 0;
            }
            else
            {
                if (rand() % 2 == 0)
                {
                    cell->type = TYPE_PREY;
                    cell->health = rand() % HEALTH_START;
                }
                else
                {
                    cell->type = TYPE_PREDATOR;
                    cell->health = rand() % HEALTH_START;
                }
            }
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
            for (int x = 0; x < WIDTH; x++)
            {
                for (int y = 0; y < HEIGHT; y++)
                {
                    struct cell *cell = &cells[x + y * WIDTH];

                    if (cell->type != TYPE_EMPTY)
                    {
                        int dx = 0;
                        int dy = 0;

                        switch (rand() % 8)
                        {
                        case 0:
                            dy = -1;
                            break;
                        case 1:
                            dx = 1;
                            dy = -1;
                            break;
                        case 2:
                            dx = 1;
                            break;
                        case 3:
                            dx = 1;
                            dy = 1;
                            break;
                        case 4:
                            dy = 1;
                            break;
                        case 5:
                            dx = -1;
                            dy = 1;
                            break;
                        case 6:
                            dx = -1;
                            break;
                        case 7:
                            dx = -1;
                            dy = -1;
                            break;
                        }

                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx < 0)
                        {
                            nx = WIDTH + nx;
                        }

                        if (nx >= WIDTH)
                        {
                            nx = WIDTH - nx;
                        }

                        if (ny < 0)
                        {
                            ny = HEIGHT + ny;
                        }

                        if (ny >= HEIGHT)
                        {
                            ny = HEIGHT - ny;
                        }

                        struct cell *neighbor = &cells[nx + ny * WIDTH];

                        switch (cell->type)
                        {
                        case TYPE_PREY:
                        {
                            cell->health++;

                            if (neighbor->type == TYPE_EMPTY)
                            {
                                if (cell->health >= HEALTH_REPRODUCE)
                                {
                                    // reproduce
                                    cell->health = 1;
                                    neighbor->type = TYPE_PREY;
                                    neighbor->health = rand() % HEALTH_START;
                                }
                                else
                                {
                                    // move
                                    neighbor->type = TYPE_PREY;
                                    neighbor->health = cell->health;
                                    cell->type = TYPE_EMPTY;
                                    cell->health = 0;
                                }
                            }
                        }
                        break;
                        case TYPE_PREDATOR:
                        {
                            cell->health--;

                            if (cell->health <= 0)
                            {
                                // die
                                cell->type = TYPE_EMPTY;
                                cell->health = 0;
                            }
                            else if (neighbor->type == TYPE_PREY)
                            {
                                // reproduce
                                cell->health += neighbor->health;
                                neighbor->type = TYPE_PREDATOR;
                                neighbor->health = rand() % HEALTH_START;
                            }
                            else
                            {
                                // move
                                neighbor->type = TYPE_PREDATOR;
                                neighbor->health = cell->health;
                                cell->type = TYPE_EMPTY;
                                cell->health = 0;
                            }
                        }
                        break;
                        }
                    }
                }
            }
        }

        int num_prey = 0;
        int num_predators = 0;

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

                switch (cell->type)
                {
                case TYPE_EMPTY:
                {
                    red = 0;
                    green = 0;
                    blue = 0;
                }
                break;
                case TYPE_PREY:
                {
                    red = 0;
                    green = 255;
                    blue = 0;

                    num_prey++;
                }
                break;
                case TYPE_PREDATOR:
                {
                    red = 255;
                    green = 0;
                    blue = 0;

                    num_predators++;
                }
                break;
                }

                *pixel = ((red & 0xff) << 24) | ((green & 0xff) << 16) | ((blue & 0xff) << 8) | ((alpha & 0xff) << 0);
            }
        }

        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "%s - Prey: %d, Predators: %d", TITLE, num_prey, num_predators);
        SDL_SetWindowTitle(window, buffer);

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
