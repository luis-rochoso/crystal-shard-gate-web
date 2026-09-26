#include <bits/stdc++.h>
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

InputConnector* dragged = nullptr;

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

std::unordered_map<std::string, Texture2D> textures;


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
            dragged = &connector;
            dragged->isConnected = false;
            return true;
        }
    }
    return false;
}

bool releasedOverOutputConnector() {
    // Checks if the link was released over an output hook
    for (auto& [label, connector] : outputs) {
        if (CheckCollisionPointRec(mousePoint, connector.hook) and IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragged->linked = &connector; // Input's connected only to this output
            dragged->isConnected = true;
            connector.linkeds.push_back(dragged); // Output adds this input to its list of connections
            return true;
        }
    }
    return false;
}

void dragRelay(Relay &relay) {

    relay.center = mousePoint;
    relay.RefreshOriginPosition();

    if (relay.origin.x < PLATE_X) {
        relay.origin.x = PLATE_X;
    }
    if (relay.origin.x + RELAY_WIDTH > PLATE_X + PLATE_WIDTH) {
        relay.origin.x = PLATE_X + PLATE_WIDTH - RELAY_WIDTH;
    }
    if (relay.origin.y < PLATE_Y) {
        relay.origin.y = PLATE_Y;
    }
    if (relay.origin.y + RELAY_HEIGHT > PLATE_Y + PLATE_HEIGHT) {
        relay.origin.y = PLATE_Y + PLATE_HEIGHT - RELAY_HEIGHT;
    }

}