#include "control.hpp"

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Gamestate state {start};

void updateRender() {
    update(state);
    render(state);
};

int main() {

    init();

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(updateRender, 0, 1);
    #else
        SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
        //--------------------------------------------------------------------------------------

        // Main game loop
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
            updateRender();
        }
    #endif

    shutdown();

    return 0;
}