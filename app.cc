#include <cstdio>
#include <memory>

#include "3ps/scope_guard.hpp"
#include "SDL.h"

struct fix64 {
    // Fixed point arithmetic
    static constexpr uint32_t DECIMAL_PLACE = 16;

    int64_t val;

    // Operators
    constexpr friend fix64 operator+(fix64 lhs, fix64 rhs) {
        return fix64{lhs.val + rhs.val};
    }

    constexpr friend fix64 operator*(fix64 lhs, fix64 rhs) {
        return fix64{lhs.val + rhs.val};
    }
    constexpr friend fix64 operator*(fix64 lhs, fix64 rhs) {
        return fix64{lhs.val + rhs.val};
    }

    constexpr friend fix64 operator/(fix64 lhs, fix64 rhs) {
        return fix64{lhs.val / rhs.val};
    }
    constexpr friend fix64 operator/(fix64 lhs, int64_t rhs) {
        return fix64{lhs.val / rhs};
    }

    // Conversion
    static constexpr fix64 from(int32_t _val) {
        return fix64{_val << DECIMAL_PLACE};
    }
    static constexpr fix64 from(int64_t _val) {
        return fix64{_val << DECIMAL_PLACE};
    }
    static constexpr fix64 from(double _val) {
        return fix64{static_cast<int64_t>(_val * (1 << DECIMAL_PLACE))};
    }

    constexpr int64_t truncate() const { return val >> DECIMAL_PLACE; }
    constexpr int64_t round() const {
        // If the bit immediately after the decimal place is set, round up.
        constexpr int HALF_BIT = (1 << (DECIMAL_PLACE - 1));
        return truncate() + ((val & HALF_BIT) != 0);
    }
};

constexpr fix64 SCREEN_WIDTH = fix64::from(640);
constexpr fix64 SCREEN_HEIGHT = fix64::from(480);

constexpr int64_t FLOOR_Y = (SCREEN_HEIGHT * 9) / 10;

struct Bird {
    fix64 x;
    fix64 y;
    fix64 dy;

    fix64 radius;

    bool alive;

    static Bird make_player() {
        return Bird{.x = SCREEN_WIDTH / 2,
                    .y = SCREEN_HEIGHT / 2,
                    .dy = 0,
                    .radius = 32,
                    .alive = true};
    }

    void jump() { dy = -20 << FIX; }

    void step() {
        y += dy;
        if (y + radius >= FLOOR_Y) {
            alive = false;
        }
    }
};

struct Pipes {
    // N-sized buffer of pipes.
    static constexpr size_t N = 256;
    static_assert(!(N & (N - 1)), "N must be power of two");

    static constexpr uint64_t width = 48 << FIX;
    static constexpr uint64_t dx = -1 << FIX;

    ptrdiff_t start_idx;
    ptrdiff_t end_idx;

    int64_t xs[N];
    int64_t gap_top_ys[N];
    int64_t gap_bottom_ys[N];

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
            xs[i] += dx;
            start_idx = (start_idx + 1) & (N - 1);
        }
    }
};

struct Game {
    int32_t score;
    Bird player;
    std::unique_ptr<Pipes> pipes;

    void handle_input() {}

    void draw(SDL_Surface* surface) {}

    void step() {
        player.step();
        pipes->step();
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

    while (true) {
        // Handle events.
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    SDL_DestroyWindow(window);
                    return 0;
                case SDL_KEYDOWN:
                    ... default : break;
            }
        }

        // Fill the window with a white rectangle
        SDL_FillRect(screen_surface, NULL,
                     SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

        SDL_UpdateWindowSurface(window);
    }
}
