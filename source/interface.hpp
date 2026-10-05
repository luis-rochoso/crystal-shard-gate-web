#include "relay.hpp"
#include "states.hpp"
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

const float PLATE_X = SCREEN_WIDTH / 16;
const float PLATE_Y = SCREEN_HEIGHT / 10;
const float PLATE_WIDTH = PLATE_X * 14;
const float PLATE_HEIGHT = PLATE_Y * 8;

const float buttonRadius = 25;

size_t frameCounter = 0;

struct Circle {
    Vector2 center;
    float radius;
};

// 50 is half the shard texture's height
Vector2 buttonCenter[2] = {{PLATE_X - (PLATE_WIDTH/16), PLATE_Y + 50},
                           {PLATE_X - (PLATE_WIDTH/16), PLATE_Y + (PLATE_HEIGHT/3) + 50}};

Circle shardButtons[2] = {{buttonCenter[0], buttonRadius},
                          {buttonCenter[1], buttonRadius}};

Vector2 screwCenter[4] = {
        {PLATE_X + PLATE_WIDTH / 8, PLATE_Y + PLATE_HEIGHT / 4},
        {PLATE_X + PLATE_WIDTH - PLATE_WIDTH / 8, PLATE_Y + PLATE_HEIGHT / 4},
        {PLATE_X + PLATE_WIDTH / 8, PLATE_Y + PLATE_HEIGHT - PLATE_HEIGHT / 4},
        {PLATE_X + PLATE_WIDTH - PLATE_WIDTH / 8, PLATE_Y + PLATE_HEIGHT - PLATE_HEIGHT / 4}
    };

bool shardLight[2] = {false, false};
bool exitLight = false;

void drawMessage(Gamestate state) {

    const char openingText[324] {
        "You descended into the sewers of the greatest city\nin the world.\n\nA map to a forgotten dungeon, buried by the ever-growing\nmetropolis above, is your only guide amidst the dark and wet\ntunnels.\n\nEventually, your way is blocked by a heavy metal hatch.\nIt won't budge, but you find its control panel on the wall to\nyour right."
    };
    const char endingText[272] {
        "The hatch begins to open with a menacing and metallic creak,\nas the four locks that kept it shut were released.\n\nBeyond it lies a dusty and even darker stairway, leading to\nunfathomable depths.\n\nYour curiosity drags you further in, for your adventure has\nonly just begun."
    };

    switch (state)
    {
    case start:
        DrawText(TextSubtext(openingText, 0, frameCounter/10), 100, 100, 20, GREEN);
        break;
    
    case end:
        DrawText(TextSubtext(endingText, 0, frameCounter/10), 100, 100, 20, GREEN);
        break;
    
    default:
        break;
    }

}

void drawBackground() {
    ClearBackground(GRAY);
}

void drawClosedPlate(bool* screws) {
    Rectangle plate {PLATE_X, PLATE_Y, PLATE_WIDTH, PLATE_HEIGHT};
    

    DrawRectangle(plate.x, plate.y, plate.width, plate.height, LIGHTGRAY);
    DrawRectangleLinesEx(plate, 10, BLACK);

    for (int i = 0; i < 4; ++i) {
        if (screws[i]) {
            // draw screw
            DrawCircleV(screwCenter[i], buttonRadius, BLACK);
            DrawCircleV(screwCenter[i], buttonRadius - 5, DARKGRAY);
        }
        else {
            // draw hole
            DrawCircleV(screwCenter[i], buttonRadius - 5, BLACK);
        }
    }
}

void drawOpenPlate() {

    Rectangle plate {PLATE_X, PLATE_Y, PLATE_WIDTH, PLATE_HEIGHT};

    DrawRectangle(plate.x, plate.y, plate.width, plate.height, BLACK);
    DrawRectangleLinesEx(plate, 10, DARKGRAY);

}

void drawCrystals(Texture2D shard) {

    // Crystal 1 (switchable)
    if (shardLight[0]) {
        DrawTexture(shard, PLATE_X, PLATE_Y, RAYWHITE);
    }
    else {
        DrawTexture(shard, PLATE_X, PLATE_Y, GRAY);
    }

    // Crystal 2 (switchable)
    if (shardLight[1]) {
        DrawTexture(shard, PLATE_X, PLATE_Y + (PLATE_HEIGHT/3), RAYWHITE);
    }
    else {
        DrawTexture(shard, PLATE_X, PLATE_Y + (PLATE_HEIGHT/3), GRAY);
    }

    // Crystal 3 (always on)
    DrawTexture(shard, PLATE_X, PLATE_Y + ((PLATE_HEIGHT/3) * 2), RAYWHITE);

}

void drawButtons(Texture2D crack) {

    // Button 1
    DrawCircleV(buttonCenter[0], buttonRadius + 3, BLACK);
    DrawCircleV(buttonCenter[0], buttonRadius, shardLight[0] ? GREEN : RED);


    // Button 2
    DrawCircleV(buttonCenter[1], buttonRadius + 3, BLACK);
    DrawCircleV(buttonCenter[1], buttonRadius, shardLight[1] ? GREEN : RED);

    // Broken button
    Vector2 brokenButtonCenter = {buttonCenter[1].x, buttonCenter[1].y + 120};
    DrawCircleV(brokenButtonCenter, buttonRadius + 6, BLACK);
    DrawCircleV(brokenButtonCenter, buttonRadius, DARKGRAY);

    DrawTexture(crack, brokenButtonCenter.x - buttonRadius + 3, brokenButtonCenter.y - buttonRadius, RAYWHITE);


    // Output signal
    DrawCircle(PLATE_WIDTH + 75, buttonCenter[1].y, buttonRadius / 2, exitLight ? GREEN : RED);
}

void drawSymbols(Texture2D delta, Texture2D gamma, Texture2D sigma) {

    DrawTexture(gamma, buttonCenter[0].x - 20, buttonCenter[0].y - 30, RAYWHITE);
    DrawTexture(sigma, buttonCenter[1].x - 8, buttonCenter[1].y - 30, RAYWHITE);

    DrawTexture(delta, PLATE_WIDTH + 62.5f, buttonCenter[1].y - 50, RAYWHITE);

}

void drawRelay(Relay relay) {

    Rectangle mainframe = {relay.origin.x, relay.origin.y, RELAY_WIDTH, RELAY_HEIGHT};

    // Body
    DrawRectangleRec(mainframe, GRAY);
    DrawRectangleLinesEx(mainframe, 3, WHITE);

    // Connector indicators
    DrawCircleSector(relay.control->hookCenter, buttonRadius / 2, 270.0f, 90.0f, 1, ORANGE);
    DrawCircleSector(relay.input->hookCenter, buttonRadius / 2, 270.0f, 90.0f, 1, ORANGE);

    DrawCircleV(relay.output->hookCenter, buttonRadius / 2, PINK);

    // Interior Wiring
    Color wireColor = relay.input->isPowered ? GREEN : RED;
    if (relay.defaultMode == true) {
        if (relay.control->isPowered) {
            DrawLineBezier(relay.input->hookCenter, relay.center, 2.0f, wireColor);
        }
        else {
            DrawLineBezier(relay.input->hookCenter, relay.output->hookCenter, 2.0f, wireColor);
        }
    }
    else {
        if (relay.control->isPowered) {
            DrawLineBezier(relay.input->hookCenter, relay.output->hookCenter, 2.0f, wireColor);
        }
        else {
            DrawLineBezier(relay.input->hookCenter, relay.center, 2.0f, wireColor);
        }
    }
}

void drawLockLights(bool* locks) {

    const float LIGHT_X = PLATE_WIDTH + 75 - buttonRadius / 2;
    const float LOCKLIGHT_WIDTH = buttonRadius;
    const float LOCKLIGHT_HEIGHT = (SCREEN_HEIGHT / 60);

    Rectangle lockLights[4] = {
        {LIGHT_X, 50, LOCKLIGHT_WIDTH, LOCKLIGHT_HEIGHT},
        {LIGHT_X, 80, LOCKLIGHT_WIDTH, LOCKLIGHT_HEIGHT},
        {LIGHT_X, 110, LOCKLIGHT_WIDTH, LOCKLIGHT_HEIGHT},
        {LIGHT_X, 140, LOCKLIGHT_WIDTH, LOCKLIGHT_HEIGHT} 
    };

    for (int i = 0; i < 4; ++i) {
        DrawRectangleRec(lockLights[i], locks[i] ? GREEN : RED);
        DrawRectangleLinesEx(lockLights[i], 2, BLACK);
    }

}