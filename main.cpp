#include <iostream>
#include <SDL2/SDL.h>
#include <experimental/random>
#include "include/circle.hpp"
#include <vector>

const int WIDTH = 1360, HEIGHT = 760;
const int H_WIDTH = WIDTH / 2;
const int H_HEIGHT = HEIGHT / 2;
const int STARS_COUNT = 200;
float speed = 0.1;
float radius_delta = 0.001;
Uint32 fps_lasttime = SDL_GetTicks();
Uint32 fps_frames = 0;

struct Star
{
    float x, y, z, radius, brightness, viewX, viewY;

    Star()
    {
        newStar();
    }

    void newStar()
    {
        x = viewX = std::experimental::randint(0, WIDTH) - H_WIDTH;
        y = viewY = std::experimental::randint(0, HEIGHT) - H_HEIGHT;
        z = 256;
        brightness = 0.0;
        radius = 1.0;
    }

    bool isOffScreen()
    {
        return z <= 0 || viewX <= -H_WIDTH || viewY <= -H_HEIGHT || viewX >= H_WIDTH || viewY >= H_HEIGHT;
    }

    void processStar()
    {
        viewX = std::round(x * 256 / z);
        viewY = std::round(y * 256 / z);
        z -= speed;
        radius += radius_delta;
        if (isOffScreen())
        {
            newStar();
        }
        if (brightness < 256)
        {
            brightness += 0.15;
        }
    }
};

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

    std::vector<Star> stars(STARS_COUNT, Star());

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
            Star *star = &stars[i];
            star->processStar();
            SDL_SetRenderDrawColor(renderer, star->brightness, star->brightness, star->brightness, 255);
            int x = star->viewX + H_WIDTH;
            int y = star->viewY + H_HEIGHT;
            SDL_RenderFillCircle(renderer, x, y, std::round(star->radius));
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}