#pragma once

#include "raylib.h"
#include <imgui.h>

struct FocusData
{
    bool prevWindowFocused;
    bool windowFocused;
    bool mouseInWindow;
    bool isFirstUnfocus;

    void ObserveFocus();
    void Update();
};