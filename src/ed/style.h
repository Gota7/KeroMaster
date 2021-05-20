#pragma once

#include "imgui.h"
#include <string>

// For styling the editor.
struct EditorStyle
{
    std::string name;
    ImGuiStyle style;

    void Load(std::string name);
    void Save();
};