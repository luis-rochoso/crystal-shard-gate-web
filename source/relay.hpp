#ifndef RELAY_HPP
#define RELAY_HPP

#include "raylib.h"
#include "connectors.hpp"

const float RELAY_WIDTH = 150;
const float RELAY_HEIGHT = 100;

struct Relay {

    InputConnector* control;
    InputConnector* input;
    InputConnector::OutputConnector* output;

    bool defaultMode;

    void powerRelay() {
        if (defaultMode == true) {
            output->isPowered = !control->isPowered and input->isPowered;
        }
        else {
            output->isPowered = control->isPowered and input->isPowered;
        }
    }

    // Position
    Vector2 origin {0, 0};
    Vector2 center;
    Rectangle hitbox {origin.x, origin.y, RELAY_WIDTH, RELAY_HEIGHT};

    void RefreshOriginPosition() {
        origin.x = center.x - RELAY_WIDTH / 2;
        origin.y = center.y - RELAY_HEIGHT / 2;
    }

    void refreshHookPositions() {
        hitbox = {origin.x, origin.y, RELAY_WIDTH, RELAY_HEIGHT};

        control->hook = {origin.x - 10, origin.y + 12.5f, 25, 25};
        control->hookCenter = {control->hook.x + (control->hook.width / 2),
                               control->hook.y + (control->hook.height / 2)};

        input->hook = {origin.x - 10, origin.y + 62.5f, 25, 25};
        input->hookCenter = {input->hook.x + (input->hook.width / 2),
                             input->hook.y + (input->hook.height / 2)};

        output->hook = {origin.x + RELAY_WIDTH - 10, origin.y + 37.5f, 25, 25};
        output->hookCenter = {output->hook.x + (output->hook.width / 2),
                              output->hook.y + (output->hook.height / 2)};
    }

    bool mouseOverConnectors(Vector2 mousepoint) {
        if (CheckCollisionPointRec(mousepoint, control->hook)) {return true;}
        if (CheckCollisionPointRec(mousepoint, input->hook)) {return true;}
        if (CheckCollisionPointRec(mousepoint, output->hook)) {return true;}

        return false;
    }
};

#endif