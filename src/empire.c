#include <malloc.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_TITLE "Empire"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS_CAP 30
#define FRAME_DELAY (1000 / FPS_CAP)

#define LIFESPAN 100
#define REPRODUCTION_THRESHOLD 10
#define MUTATION_PERCENTAGE 1

struct cell
{
    unsigned int colony_index;
    int age;
    float strength;
    int reproduction_value;
    bool alive;
};

struct colony
{
    const char *name;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
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
    struct colony colonies[] = {
        {"Red", 255, 0, 0},
        {"Orange", 255, 127, 0},
        {"Yellow", 255, 255, 0},
        {"Green", 0, 255, 0},
        {"Blue", 0, 0, 255},
        {"Violet", 127, 0, 255},
        {"Cyan", 0, 255, 255},
        {"Magenta", 255, 0, 255},
        {"White", 255, 255, 255}
    };

start:
    memset(cells, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));

    for (unsigned int i = 0; i < sizeof(colonies) / sizeof(struct colony); i++)
    {
        int x = rand() % WINDOW_WIDTH;
        int y = rand() % WINDOW_HEIGHT;
        struct cell *cell = &cells[x + y * WINDOW_WIDTH];

        cell->colony_index = i;
        cell->strength = RAND_MAX;
        cell->alive = true;
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
                case SDLK_p:
                {
                    printf("---\n");

                    for (unsigned int i = 0; i < sizeof(colonies) / sizeof(struct colony); i++)
                    {
                        struct colony *colony = &colonies[i];

                        int total = 0;
                        float average_age = 0;
                        float average_strength = 0;

                        for (int x = 0; x < WINDOW_WIDTH; x++)
                        {
                            for (int y = 0; y < WINDOW_HEIGHT; y++)
                            {
                                struct cell *cell = &cells[x + y * WINDOW_WIDTH];

                                if (cell->alive && cell->colony_index == i)
                                {
                                    total++;
                                    average_age += cell->age;
                                    average_strength += cell->strength;
                                }
                            }
                        }

                        if (total > 0)
                        {
                            average_age /= total;
                            average_strength /= total;
                        }

                        printf(
                            "%s\tTotal: %d\tAvg Age: %f\tAvg Str:%f\n",
                            colony->name,
                            total,
                            average_age,
                            average_strength);
                    }

                    printf("---\n");
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
            struct cell *new_cells = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));
            memcpy(new_cells, cells, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));

            for (int x = 0; x < WINDOW_WIDTH; x++)
            {
                for (int y = 0; y < WINDOW_HEIGHT; y++)
                {
                    struct cell *cell = &cells[x + y * WINDOW_WIDTH];
                    struct cell *new_cell = &new_cells[x + y * WINDOW_WIDTH];

                    if (cell->alive)
                    {
                        cell->age++;
                        cell->reproduction_value++;

                        if (cell->age > LIFESPAN)
                        {
                            cell->alive = false;

                            continue;
                        }

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
                        struct cell *new_neighbor = &cells[nx + ny * WINDOW_WIDTH];

                        if (neighbor->alive)
                        {
                            if (neighbor->colony_index != cell->colony_index)
                            {
                                // fight
                                if (neighbor->strength >= cell->strength)
                                {
                                    cell->alive = false;
                                }
                                else
                                {
                                    neighbor->alive = false;
                                }
                            }
                        }
                        else
                        {
                            if (cell->reproduction_value > REPRODUCTION_THRESHOLD)
                            {
                                // reproduce
                                cell->reproduction_value = 0;

                                neighbor->colony_index = cell->colony_index;
                                neighbor->age = 0;
                                neighbor->strength = cell->strength;
                                neighbor->reproduction_value = 0;
                                neighbor->alive = true;

                                int mutation = rand() % 100;
                                if (mutation < MUTATION_PERCENTAGE)
                                {
                                    neighbor->strength *= (float)(rand() % 100) / 100.0f;
                                }
                            }
                            else
                            {
                                // move
                                neighbor->colony_index = cell->colony_index;
                                neighbor->age = cell->age;
                                neighbor->strength = cell->strength;
                                neighbor->reproduction_value = cell->reproduction_value;
                                neighbor->alive = cell->alive;

                                cell->colony_index = 0;
                                cell->age = 0;
                                cell->strength = 0;
                                cell->reproduction_value = 0;
                                cell->alive = false;
                            }
                        }
                    }
                }
            }

            // memcpy(cells, new_cells, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(struct cell));
            free(new_cells);
        }

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
                    struct colony *colony = &colonies[cell->colony_index];

                    red = (int)((float)colony->red * (cell->strength / (float)RAND_MAX));
                    green = (int)((float)colony->green * (cell->strength / (float)RAND_MAX));
                    blue = (int)((float)colony->blue * (cell->strength / (float)RAND_MAX));
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

    free(cells);

    free(pixels);

    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
