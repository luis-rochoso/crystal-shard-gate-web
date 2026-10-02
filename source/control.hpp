#include "controlutils.hpp"

void init() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Controle da Escotilha");

    SetTargetFPS(60);

    loadTextures();

    buildRelays();
    buildConnectors();
}

void update(Gamestate &state) {

    mousePoint = GetMousePosition();
    int holeCounter = 0;
    InputConnector::OutputConnector target;

    dragLineStartPoint = { 0.0f, 0.0f };
    dragLineEndPoint = { 0.0f, 0.0f };

    switch (state) {

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
    
    default:
        break;
    }

}

void render(Gamestate &state) {

    BeginDrawing();
    switch (state)
    {
    case closed:
        drawBackground();
        drawClosedPlate(screws);
        drawButtons(textures["crack"]);
        drawSymbols(textures["delta"], textures["gamma"], textures["sigma"]);
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
    CloseWindow();
}