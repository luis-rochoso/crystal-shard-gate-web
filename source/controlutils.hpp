#include <unordered_map>
#include <string>
#include "states.hpp"
#include "connectors.hpp"
#include "relay.hpp"
#include "interface.hpp"

bool operator==(Vector2 a, Vector2 b) {
    return (a.x == b.x) and (a.y == b.y);
}

std::unordered_map<std::string, InputConnector> inputs;
std::unordered_map<std::string, InputConnector::OutputConnector> outputs;

Relay defaultOn;
Relay defaultOff;
Relay* draggedRelay = nullptr;

InputConnector* draggedConnector = nullptr;

struct Link {
    Vector2 start;
    Vector2 end;
};

Vector2 mousePoint { 0.0f, 0.0f };
Vector2 dragLineStartPoint = { 0.0f, 0.0f };
Vector2 dragLineEndPoint { 0.0f, 0.0f };

bool* screws = new bool[4] {true, true, true, true};
bool shardPower[2] = {false, false};

bool exitPower = false;
bool* locks = new bool[4] {false, false, false, false};

bool newGamePlus = false;

int waitUntilFrame = 0;
int waitDuration = 0;

std::unordered_map<std::string, Texture2D> textures;

void loadTextures() {

    Image shard = LoadImage("/home/rochoso/Desktop/sideProjects/eberron-shard-gate/assets/dragonshard.png");
    ImageResize(&shard, 100, 100);

    SetWindowIcon(shard);

    textures["shard"] = LoadTextureFromImage(shard);

    UnloadImage(shard);

    Image delta = LoadImage("/home/rochoso/Desktop/sideProjects/eberron-shard-gate/assets/delta.png");
    ImageResize(&delta, 25, 25);
    Image gamma = LoadImage("/home/rochoso/Desktop/sideProjects/eberron-shard-gate/assets/gamma.png");
    ImageResize(&gamma, 50, 50);
    Image sigma = LoadImage("/home/rochoso/Desktop/sideProjects/eberron-shard-gate/assets/sigma.png");
    ImageResize(&sigma, 25, 50);

    textures["delta"] = LoadTextureFromImage(delta);
    textures["gamma"] = LoadTextureFromImage(gamma);
    textures["sigma"] = LoadTextureFromImage(sigma);

    UnloadImage(delta);
    UnloadImage(gamma);
    UnloadImage(sigma);

    Image crack = LoadImage("/home/rochoso/Desktop/sideProjects/eberron-shard-gate/assets/crack.png");
    ImageResize(&crack, 50, 50);
    textures["crack"] = LoadTextureFromImage(crack);
    UnloadImage(crack);
    
}


// Handling pressing crystal buttons
void checkButtonPress() {
    for (int i = 0; i < 2; ++i) {
        if (CheckCollisionPointCircle(mousePoint, shardButtons[i].center, buttonRadius)) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                shardPower[i] = !shardPower[i];
            }
        }    
    }
}

// Lighting up or out the toggable shards  
void toggleLights() {
    for (int i = 0; i < 2; ++i) {
        shardLight[i] = shardPower[i];
    }
    exitLight = exitPower;
}

void updateLocks(Gamestate state) {

    if (state == draggingWire) {
        for (int i = 0; i < 4; ++i) {
            locks[i] = false;
        }
        return;
    }

    // not A and not B
    if (!outputs["shard0"].isPowered and !outputs["shard1"].isPowered) {
        if (exitPower) locks[0] = true;
    }
    // not A and B
    else if (!outputs["shard0"].isPowered and outputs["shard1"].isPowered) {
        if (exitPower) locks[1] = true;
    }
    // A and not B
    else if (outputs["shard0"].isPowered and !outputs["shard1"].isPowered) {
        if (exitPower) locks[2] = true;
    }
    // A and B
    else if (outputs["shard0"].isPowered and outputs["shard1"].isPowered) {
        if (exitPower) locks[3] = false;
        else locks[3] = true;
        
    }

}

void buildRelays() {
    defaultOn.origin = {500, 250};
    defaultOn.center = {575, 300};
    defaultOn.defaultMode = true;

    defaultOff.origin = {300, 270};
    defaultOff.center = {375, 320};
    defaultOff.defaultMode = false;
}

void buildConnectors() {

    // Shard connectors
    outputs["shard0"].hook = {PLATE_X + 80, shardButtons[0].center.y - 12.5f, 25, 25};
    outputs["shard1"].hook = {PLATE_X + 80, shardButtons[1].center.y - 12.5f, 25, 25};
    outputs["shard2"].hook = {PLATE_X + 80, shardButtons[1].center.y + 120 - 12.5f, 25, 25};

    outputs["shard0"].hookCenter = {outputs["shard0"].hook.x + (outputs["shard0"].hook.width / 2),
                                    outputs["shard0"].hook.y + (outputs["shard0"].hook.height / 2)};
    outputs["shard1"].hookCenter = {outputs["shard1"].hook.x + (outputs["shard1"].hook.width / 2),
                                    outputs["shard1"].hook.y + (outputs["shard1"].hook.height / 2)};
    outputs["shard2"].hookCenter = {outputs["shard2"].hook.x + (outputs["shard2"].hook.width / 2),
                                    outputs["shard2"].hook.y + (outputs["shard2"].hook.height / 2)};
    
    // Relay connectors
    inputs["rOnControl"].hook = {defaultOn.origin.x - 10, defaultOn.origin.y + 12.5f, 25, 25};
    defaultOn.control = &inputs["rOnControl"];
    inputs["rOnInput"].hook = {defaultOn.origin.x - 10, defaultOn.origin.y + 62.5f, 25, 25};
    defaultOn.input = &inputs["rOnInput"];
    outputs["rOnOut"].hook = {defaultOn.origin.x + RELAY_WIDTH - 10, defaultOn.origin.y + 37.5f, 25, 25};
    defaultOn.output = &outputs["rOnOut"];

    inputs["rOffControl"].hook = {defaultOff.origin.x - 10, defaultOff.origin.y + 12.5f, 25, 25};
    defaultOff.control = &inputs["rOffControl"];
    inputs["rOffInput"].hook = {defaultOff.origin.x - 10, defaultOff.origin.y + 62.5f, 25, 25};
    defaultOff.input = &inputs["rOffInput"];
    outputs["rOffOut"].hook = {defaultOff.origin.x + RELAY_WIDTH - 10, defaultOff.origin.y + 37.5f, 25, 25};
    defaultOff.output = &outputs["rOffOut"];

    // Exit connector
    inputs["exit"].hook = {PLATE_X + PLATE_WIDTH - 20, shardButtons[1].center.y - 12.5f, 25, 25};
    inputs["exit"].hookCenter = {inputs["exit"].hook.x + (inputs["exit"].hook.width / 2),
                                 inputs["exit"].hook.y + (inputs["exit"].hook.height / 2)};
}

void powerConnectors() {

    outputs["shard0"].isPowered = shardPower[0];
    outputs["shard1"].isPowered = shardPower[1];
    outputs["shard2"].isPowered = true;

    for (auto& [label, connector] : inputs) {
        
        if (connector.isConnected) {
            connector.isPowered = connector.linked->isPowered;
        }
        else {
            connector.isPowered = false;
        }

    }

    exitPower = inputs["exit"].isPowered;

}

bool clickedInputConnector() {
    // Checks if the player clicked on an input hook
    for (auto& [label, connector] : inputs) {
        if (CheckCollisionPointRec(mousePoint, connector.hook) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            draggedConnector = &connector;
            draggedConnector->isConnected = false;
            return true;
        }
    }
    return false;
}

bool releasedOverOutputConnector() {
    // Checks if the link was released over an output hook
    for (auto& [label, connector] : outputs) {
        if (CheckCollisionPointRec(mousePoint, connector.hook) and IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            draggedConnector->linked = &connector; // Input's connected only to this output
            draggedConnector->isConnected = true;
            connector.linkeds.push_back(draggedConnector); // Output adds this input to its list of connections
            return true;
        }
    }
    return false;
}

void dragRelay(Relay* relay) {

    relay->center = mousePoint;
    relay->RefreshOriginPosition();

    if (relay->origin.x < PLATE_X) {
        relay->origin.x = PLATE_X;
    }
    if (relay->origin.x + RELAY_WIDTH > PLATE_X + PLATE_WIDTH) {
        relay->origin.x = PLATE_X + PLATE_WIDTH - RELAY_WIDTH;
    }
    if (relay->origin.y < PLATE_Y) {
        relay->origin.y = PLATE_Y;
    }
    if (relay->origin.y + RELAY_HEIGHT > PLATE_Y + PLATE_HEIGHT) {
        relay->origin.y = PLATE_Y + PLATE_HEIGHT - RELAY_HEIGHT;
    }

    // calculate the relay's center again because moving the mouse near/past the
    // panel borders causes it to not update correctly
    relay->center = {relay->origin.x + RELAY_WIDTH / 2, relay->origin.y + RELAY_HEIGHT / 2};
}