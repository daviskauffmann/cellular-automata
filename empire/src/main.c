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

#define MAX_STRENGTH 100
#define REPRODUCTION_THRESHOLD 20

struct cell
{
    int age;
    int strength;
    int reproduction_value;
    bool alive;
    bool diseased;
    int colony_index;
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
    struct colony colonies[] = {
        {"Red", 255, 0, 0},
        {"Green", 0, 255, 0},
        {"Blue", 0, 0, 255},
        {"Yellow", 255, 255, 0},
        {"Cyan", 0, 255, 255},
        {"Magenta", 255, 0, 255},
        {"White", 255, 255, 255}};

start:
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            struct cell *cell = &cells[x + y * WIDTH];

            cell->age = 0;
            cell->strength = rand() % MAX_STRENGTH;
            cell->reproduction_value = 0;
            cell->alive = rand() % 100000 == 0;
            cell->diseased = false;
            cell->colony_index = rand() % (sizeof(colonies) / sizeof(struct colony));
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
                case SDLK_p:
                {
                    for (int i = 0; i < sizeof(colonies) / sizeof(struct colony); i++)
                    {
                        struct colony *colony = &colonies[i];

                        int total = 0;
                        int average_strength = 0;

                        for (int x = 0; x < WIDTH; x++)
                        {
                            for (int y = 0; y < HEIGHT; y++)
                            {
                                struct cell *cell = &cells[x + y * WIDTH];

                                if (cell->alive && cell->colony_index == i)
                                {
                                    total++;
                                    average_strength += cell->strength;
                                }
                            }
                        }

                        if (total > 0)
                        {
                            average_strength /= total;
                        }

                        printf("%s\tTotal: %d\tAvg Str:%d\n", colony->name, total, average_strength);
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
            for (int x = 0; x < WIDTH; x++)
            {
                for (int y = 0; y < HEIGHT; y++)
                {
                    struct cell *cell = &cells[x + y * WIDTH];

                    if (cell->alive)
                    {
                        cell->age += cell->diseased ? 2 : 1;

                        if (cell->age > cell->strength)
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

                        cell->reproduction_value++;

                        if (neighbor->alive)
                        {
                            if (neighbor->colony_index == cell->colony_index)
                            {
                                if (neighbor->diseased)
                                {
                                    cell->diseased = rand() % 2 == 0;
                                }
                            }
                            else
                            {
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
                                cell->reproduction_value = 0;
                                neighbor->age = 0;
                                neighbor->strength = cell->strength;
                                neighbor->reproduction_value = 0;
                                neighbor->alive = true;
                                neighbor->diseased = cell->diseased ? rand() % 2 == 0 : false;
                                neighbor->colony_index = cell->colony_index;

                                if (rand() % 100 == 0)
                                {
                                    neighbor->strength *= (rand() % 200) / 100;
                                }

                                if (rand() % 1000 == 0)
                                {
                                    neighbor->diseased = true;
                                }
                            }
                            else
                            {
                                neighbor->age = cell->age;
                                neighbor->strength = cell->strength;
                                neighbor->reproduction_value = cell->reproduction_value;
                                neighbor->alive = cell->alive;
                                neighbor->diseased = cell->diseased;
                                neighbor->colony_index = cell->colony_index;
                                cell->age = 0;
                                cell->strength = 0;
                                cell->reproduction_value = 0;
                                cell->alive = false;
                                cell->diseased = false;
                                cell->colony_index = 0;
                            }
                        }
                    }
                }
            }
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
                    struct colony *colony = &colonies[cell->colony_index];

                    red = colony->red;
                    green = colony->green;
                    blue = colony->blue;
                }
                else
                {
                    red = 0;
                    green = 0;
                    blue = 0;
                }

                // if (cell->diseased)
                // {
                //     red /= 2;
                //     green /= 2;
                //     blue /= 2;
                // }

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
