#pragma once

#include "imgui.h"
#include <string>

// For styling the editor.
struct EditorStyle
{
    std::string name; // Style name.
    ImGuiStyle style; // Actual style data.

    // Load a style from a style name.
    void Load(std::string name);

    // Save the style data.
    void Save();

};