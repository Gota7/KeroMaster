#pragma once

#include "tools/tool.h"
#include <string>

struct EditorNew;

// Editor settings.
struct SettingsNew
{
    int width = 1240; // Width of the window.
    int height = 720; // Height of the window.
    bool maximized = false; // If the window should be maximized.
    std::string rscPath = ""; // Resource path.
    std::string lastLevel = ""; // Name of the last level loaded.
    bool usePalette = true; // If to use the tile palette.
    ToolItem leftClick = ToolItem::CurrentTool; // Left click tool.
    ToolItem rightClick = ToolItem::Hand; // Right click tool.
    ToolItem middleClick = ToolItem::EntityHand; // Middle click tool.
    std::string currStyle; // Current style.
    bool show; // If to show the settings window.

    // Load the settings.
    void Load();

    // Save the settings.
    void Save();

    // Show the settings window.
    void DrawUI(EditorNew* ed);

};