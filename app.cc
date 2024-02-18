#include <cstdio>
#include <memory>

#include "3ps/scope_guard.hpp"
#include "SDL.h"

constexpr double SCREEN_WIDTH = 640.;
constexpr double SCREEN_HEIGHT = 480.;

constexpr double FLOOR_Y = (SCREEN_HEIGHT * 9) / 10;

struct Bird {
    double x;
    double y;
    double dy;

    double radius;

    bool alive;

    static Bird make_player() {
        return Bird{.x = SCREEN_WIDTH / 2,
                    .y = SCREEN_HEIGHT / 2,
                    .dy = 0.,
                    .radius = 32.,
                    .alive = true};
    }

    void jump() { dy = -10.; }

    void step() {
        y += dy;
        if (y + radius >= FLOOR_Y) {
            alive = false;
        }
        dy += 0.5;
    }

    void draw(SDL_Surface* surface) {
        // TODO: draw this nicer
        SDL_Rect rect{
            .x = static_cast<int>(x - radius / 2),
            .y = static_cast<int>(y - radius / 2),
            .w = static_cast<int>(radius),
            .h = static_cast<int>(radius),
        };
        SDL_FillRect(surface, &rect,
                     SDL_MapRGB(surface->format, 0xf5, 0x8b, 0x11));
    }
};

struct Pipes {
    // N-sized buffer of pipes.
    static constexpr size_t N = 256;
    static_assert(!(N & (N - 1)), "N must be power of two");

    static constexpr double width = 48;
    static constexpr double dx = -1;

    ptrdiff_t start_idx{};
    ptrdiff_t end_idx{};

    int64_t total_passed{};

    double xs[N];
    double gap_top_ys[N];
    double gap_bottom_ys[N];

    size_t size() { return (end_idx - start_idx) & (N - 1); }

    void spawn(int32_t gap_top_y, int32_t gap_bottom_y) {
        // Spawn a new pipe just off-screen.
        // If there are already N pipes, the left-most one will be deleted.
        xs[end_idx] = SCREEN_WIDTH + width;
        gap_top_ys[end_idx] = gap_top_y;
        gap_bottom_ys[end_idx] = gap_bottom_y;
        end_idx = (end_idx + 1) & (N - 1);
    }

    void step() {
        for (ptrdiff_t i = start_idx; i != end_idx; i = (i + 1) & (N - 1)) {
            if (xs[i] >= SCREEN_HEIGHT / 2 && xs[i] + dx <= SCREEN_HEIGHT / 2) {
                ++total_passed;
            }
            xs[i] += dx;
            if (xs[i] <= -width) {
                // despawn the pipe
                start_idx = (start_idx + 1) & (N - 1);
            }
        }
    }

    void draw(SDL_Surface* surface) {}

    bool collides_with(Bird& bird) {
        // TODO
        return false;
    }
};

struct Game {
    bool started;

    Bird player;
    std::unique_ptr<Pipes> pipes;

    Game()
        : started(false),
          player(Bird::make_player()),
          pipes(std::make_unique<Pipes>()) {}

    void handle_keydown(SDL_KeyboardEvent& e) {
        if (e.keysym.sym == SDLK_SPACE) {
            if (started) {
                player.jump();
            } else {
                started = true;
            }
        }
    }

    int64_t get_score() { return pipes->total_passed; }

    void draw(SDL_Surface* surface) {
        // Fill the window with a blue rectangle (the sky)
        SDL_FillRect(surface, nullptr,
                     SDL_MapRGB(surface->format, 0x11, 0xb1, 0xf5));

        player.draw(surface);
        pipes->draw(surface);
    }

    void step() {
        if (!started) {
            return;
        }

        player.step();
        pipes->step();

        if (pipes->collides_with(player)) {
            player.alive = false;
        }
    }
};

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

    Game game{};

    constexpr int32_t frameDelay = 1000 / 60;  // 60 fps
    while (true) {
        int32_t frameStart = SDL_GetTicks();

        // Handle events.
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    SDL_DestroyWindow(window);
                    return 0;
                case SDL_KEYDOWN:
                    game.handle_keydown(e.key);
                    break;
                default:
                    break;
            }
        }

        game.step();
        game.draw(screen_surface);

        SDL_UpdateWindowSurface(window);

        // This keeps us from displaying more frames than 60/Second
        if (int32_t frameTime = SDL_GetTicks() - frameStart;
            frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}
