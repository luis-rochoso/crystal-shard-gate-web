#include "control.hpp"

int main() {

    init();

    Gamestate state {start};

    while (!WindowShouldClose()) {
        update(state);
        render(state);
    }

    shutdown();

    return 0;
}