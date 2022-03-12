#pragma once

#include "tool.h"

// Move the camera around.
struct HandTool : public Tool
{
    bool inPan = false; // If currently panning.
    int oldMouseX; // Old mouse position x.
    int oldMouseY; // Old mouse position y.

    // Implementations.
    void Init(Editor* ed);
    void Activate(Editor* ed, Vector2 pos1 = { 0, 0 }, Vector2 pos2 = { 0, 0 });
    void Update(Editor* ed);

};