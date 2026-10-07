#ifndef CONNECTORS_HPP
#define CONNECTORS_HPP

#include <vector>
#include "raylib.h"

struct InputConnector {
    Rectangle hook;
    Vector2 hookCenter;
    
    bool isPowered {false};
    bool isConnected {false};

    struct OutputConnector {
    Rectangle hook;
    Vector2 hookCenter;

    bool isPowered {false};

    std::vector<InputConnector*> linkeds;
    };

    OutputConnector* linked; // member of InputConnector
};

#endif