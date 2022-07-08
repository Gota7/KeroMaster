#pragma once

#include "imgui.h"
#include <string>

// Settings.
struct Settings;

// For styling the editor.
struct EditorStyle
{
    std::string name; // Style name.
    ImGuiStyle style; // Actual style data.

    // Load a style from a style name.
    void Load(std::string name, Settings* settings);

    // Save the style data.
    void Save(Settings* settings);

};