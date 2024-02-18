#include <cstdio>

#include "3ps/scope_guard.hpp"
#include "SDL.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

int main(int argc, char* argv[]) {
    auto quit_guard = sg::make_scope_guard(SDL_Quit);
    if (int err_code = SDL_Init(SDL_INIT_VIDEO); err_code != 0) {
        printf("Error initializing. SDL_Error: %s\n", SDL_GetError());
        return err_code;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Hello world", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("Error creating window. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

    while (true) {
        // Handle events.
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_DestroyWindow(window);
                return 0;
            }
        }

        // Fill the window with a white rectangle
        SDL_FillRect(screen_surface, NULL,
                     SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

        SDL_UpdateWindowSurface(window);
    }
}
