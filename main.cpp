#include <iostream>
#include <SDL2/SDL.h>
#include <experimental/random>
#include "include/circle.hpp"

const int WIDTH = 1360, HEIGHT = 760;
const int H_WIDTH = WIDTH / 2;
const int H_HEIGHT = HEIGHT / 2;
const int STARS_COUNT = 500;
float speed = 0.1;
float radius_delta = 0.001;
Uint32 fps_lasttime = SDL_GetTicks();
Uint32 fps_frames = 0;

struct Star
{
    float x, y, z, radius, brightness, viewX, viewY;
};

Star newStar()
{
    Star star;
    star.x = star.viewX = std::experimental::randint(0, WIDTH) - H_WIDTH;
    star.y = star.viewY = std::experimental::randint(0, HEIGHT) - H_HEIGHT;
    star.z = 256;
    star.brightness = 0.0;
    star.radius = 1.0;

    return star;
}

bool isOffScreen(int x, int y, int z)
{
    return z <= 0 || x <= -H_WIDTH || y <= -H_HEIGHT || x >= H_WIDTH || y >= H_HEIGHT;
}

void setFPStitle(SDL_Window *window)
{
    fps_frames++;
    if (fps_lasttime < SDL_GetTicks() - 1000)
    {
        fps_lasttime = SDL_GetTicks();      
        auto title = "C++ SDL window: " + std::to_string(fps_frames) + " FPS";
        fps_frames = 0;
        SDL_SetWindowTitle(window, title.c_str());
    }
}

Star processStar(Star star)
{
    Star s = star;
    s.viewX = std::round(s.x * 256 / s.z);
    s.viewY = std::round(s.y * 256 / s.z);
    s.z -= speed;
    s.radius += radius_delta;
    if (isOffScreen(s.viewX, s.viewY, s.z))
    {
        s = newStar();
    }
    if (s.brightness < 256)
    {
        s.brightness += 0.15;
    }
    return s;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("C++ SDL window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL)
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        std::cout << "Could not init renderer: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Event windowEvent;

    Star stars[STARS_COUNT];
    for (int i = 0; i < STARS_COUNT; i++)
    {
        stars[i] = newStar();
    }

    while (true)
    {
        if (SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
                break;
        }

        setFPStitle(window);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < STARS_COUNT; i++)
        {
            Star star = processStar(stars[i]);
            SDL_SetRenderDrawColor(renderer, stars[i].brightness, stars[i].brightness, stars[i].brightness, 255);
            int x = star.viewX + H_WIDTH;
            int y = star.viewY + H_HEIGHT;
            SDL_RenderFillCircle(renderer, x, y, std::round(stars[i].radius));
            stars[i] = star;
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}