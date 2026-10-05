#include "control.hpp"

int main() {

    init();

    Gamestate state {closed};

    while (!WindowShouldClose()) {
        update(state);
        render(state);
    }

    shutdown();

    return 0;
}