#include "controlutils.hpp"

void init() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Controle da Escotilha");

    SetTargetFPS(60);

    loadTextures();

    buildRelays();
    buildConnectors();
}

void update(Gamestate &state) {

    bool puzzleSolved = true;
    mousePoint = GetMousePosition();
    int holeCounter = 0;
    InputConnector::OutputConnector target;

    dragLineStartPoint = { 0.0f, 0.0f };
    dragLineEndPoint = { 0.0f, 0.0f };

    switch (state) {

    case start:

        // Pressing ENTER skips the opening text
        if (IsKeyPressed(KEY_ENTER)) {
            state = closed;
            frameCounter = 0;
            break;
        }

        // Holding SPACE speeds its animation
        if (IsKeyDown(KEY_SPACE)) {
            frameCounter += 8;
        }
        else frameCounter += 2;

        break;
    
    case closed:
        for (int i = 0; i < 4; ++i) {
            if (!screws[i]) {
                ++holeCounter;
            }
        }
        if (holeCounter == 4) {
            state = open;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            if (CheckCollisionPointCircle(mousePoint, screwCenter[i], buttonRadius)
                and IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    screws[i] = false;
            }
        }
        
        break;
    
    case open:    

        for (int i = 0; i < 4; ++i) {
            if (!locks[i]) {
                puzzleSolved = false;
            }
        }

        if (not newGamePlus) {
            if (puzzleSolved) {
                state = end;
                break;
            }
        }

        if (CheckCollisionPointRec(mousePoint, defaultOn.hitbox) and IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            and !defaultOn.mouseOverConnectors(mousePoint) ) {
            draggedRelay = &defaultOn;
            state = draggingRelay;
        }
        else if (CheckCollisionPointRec(mousePoint, defaultOff.hitbox) and IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            and !defaultOff.mouseOverConnectors(mousePoint)) {
            draggedRelay = &defaultOff;
            state = draggingRelay;
        }

        defaultOn.refreshHookPositions();
        defaultOff.refreshHookPositions();

        powerConnectors();
        defaultOn.powerRelay();
        defaultOff.powerRelay();

        checkButtonPress();
        toggleLights();
        updateLocks(state);

        if (clickedInputConnector()) {state = draggingWire;}
        break;
    
    case draggingRelay:
    
        dragRelay(draggedRelay);

        defaultOn.refreshHookPositions();
        defaultOff.refreshHookPositions();

        powerConnectors();
        defaultOn.powerRelay();
        defaultOff.powerRelay();

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            draggedRelay = nullptr;
            state = open;
        }

        break;

    case draggingWire:

        updateLocks(state);

        powerConnectors();

        dragLineStartPoint = draggedConnector->hookCenter;
        dragLineEndPoint = mousePoint;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

            if (!releasedOverOutputConnector()) {
                draggedConnector = nullptr;
            }
            state = open;
        }
        break;
    
    case end:

        if (IsKeyPressed(KEY_B)) {
            state = open;
            newGamePlus = true;
        }

        // Pressing ENTER skips the text animation
        if (IsKeyPressed(KEY_ENTER)) {
            frameCounter = 2690;
            break;
        }

        // Holding SPACE speeds its animation
        if (IsKeyDown(KEY_SPACE)) {
            frameCounter += 16;
        }
        else frameCounter += 4;

        break;

    default:
        break;
    }

}

void render(Gamestate &state) {

    BeginDrawing();
    switch (state)
    {

    case start:
        
        ClearBackground(BLACK);

        drawMessage(state);
        if (frameCounter > 180 and frameCounter < 3240) {
            DrawText("[SPACE] to speed up", 10, 435, 10, GREEN);
        }
        if (frameCounter >= 3300) {
            DrawText("Press [ENTER] to look at the panel.", 100, 350, 30, GREEN);
        }

        break;

    case closed:
        drawBackground();
        drawClosedPlate(screws);
        drawButtons(textures["crack"]);
        drawSymbols(textures["delta"], textures["gamma"], textures["sigma"]);
        drawLockLights(locks);
        break;

    case open:
        drawBackground();
        drawOpenPlate();
        drawRelay(defaultOn);
        drawRelay(defaultOff);
        DrawCircleSector({PLATE_WIDTH + 40, buttonCenter[1].y}, buttonRadius / 2, 270.0f, 90.0f, 1, ORANGE);
        drawCrystals(textures["shard"]);
        drawButtons(textures["crack"]);
        drawSymbols(textures["delta"], textures["gamma"], textures["sigma"]);
        drawLockLights(locks);

        for (auto& [label, connector] : inputs) {
            if (connector.isConnected) {
                DrawLine(connector.hookCenter.x, connector.hookCenter.y,
                         connector.linked->hookCenter.x, connector.linked->hookCenter.y,
                         connector.isPowered ? GREEN : RED);
            }
        }
        
        break;

    case draggingRelay:
        
        drawBackground();
        drawOpenPlate();
        drawRelay(defaultOn);
        drawRelay(defaultOff);
        DrawCircleSector({PLATE_WIDTH + 40, buttonCenter[1].y}, buttonRadius / 2, 270.0f, 90.0f, 1, ORANGE);
        drawCrystals(textures["shard"]);
        drawButtons(textures["crack"]);
        drawSymbols(textures["delta"], textures["gamma"], textures["sigma"]);
        drawLockLights(locks);

        for (auto& [label, connector] : inputs) {
            if (connector.isConnected) {
                DrawLine(connector.hookCenter.x, connector.hookCenter.y,
                         connector.linked->hookCenter.x, connector.linked->hookCenter.y,
                         connector.isPowered ? GREEN : RED);
            }
        }

        break;
    
    case draggingWire:
        drawBackground();
        drawOpenPlate();
        drawRelay(defaultOn);
        drawRelay(defaultOff);
        DrawCircleSector({PLATE_WIDTH + 40, buttonCenter[1].y}, buttonRadius / 2, 270.0f, 90.0f, 1, ORANGE);
        drawCrystals(textures["shard"]);
        drawButtons(textures["crack"]);
        drawSymbols(textures["delta"], textures["gamma"], textures["sigma"]);
        drawLockLights(locks);

        // Draw Connectors
        for (const auto& [label, connector] : inputs) {
            DrawRectangleLinesEx(connector.hook, 3, ORANGE);
        }
        for (const auto& [label, connector] : outputs) {
            DrawRectangleLinesEx(connector.hook, 3, PINK);
        }

        // for every connected input connector, draw a line from its hookcenter to its linked hookcenter
        for (auto& [label, connector] : inputs) {
            if (connector.isConnected) {
                DrawLine(connector.hookCenter.x, connector.hookCenter.y,
                         connector.linked->hookCenter.x, connector.linked->hookCenter.y, BLUE);
            }
        }

        // Draw dragline
        DrawLineBezier(dragLineStartPoint, dragLineEndPoint, 5, ORANGE);

        break;
    
    case end:
        ClearBackground(BLACK);
        drawMessage(state);

        if (frameCounter > 2870) {
            DrawText("Thanks for playing!", 100, 300, 30, RAYWHITE);
        }
        if (frameCounter > 2990) {
            DrawText("Press [ESC] to close the game\nPress [B] to go back and mess around", 100, 350, 20, RAYWHITE);
        }
        
        break;

    default:
        break;
    }
    EndDrawing();
}

void shutdown() {
    for (auto it : textures) {
        UnloadTexture(it.second);
    }
    delete[] screws;
    delete[] locks;
    CloseWindow();
}