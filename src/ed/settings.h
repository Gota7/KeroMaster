#pragma once

#include "raylib.h"
#include <string>
#include <cstring>

using namespace std;

struct Settings
{
    string rscPath = "";
    MouseButton placeTile = MOUSE_LEFT_BUTTON;
    MouseButton removeTile = MOUSE_RIGHT_BUTTON;
    bool allowRemoveTile = true;
    MouseButton panView = MOUSE_MIDDLE_BUTTON;
    bool allowPanView = true;
};