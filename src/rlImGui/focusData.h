#pragma once

#include "raylib.h"
#include <imgui.h>

struct FocusData
{
    bool prevWindowFocused;
    bool windowFocused;
    bool mouseInWindow;
    bool isFirstUnfocus;
    bool isModal;

    void ObserveFocus();
    void Update();
};