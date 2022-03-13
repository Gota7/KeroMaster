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
    bool show = false; // If to show the settings window.
    bool showPlayArea = true; // If to show bounds of each map.
    bool showGrid = false; // Show the grid.
    bool viewLayers[3] = { true, true, true }; // What layers can be seen.
    bool viewSpriteImages = true; // If to show sprite images, if not, show the images from unit type.
    bool viewEntityBoxes = true; // Show the boxes for entities.
    bool viewEntities = true; // Show entities.
    bool viewTileAttributes = false; // If to view tile attributes.

    // Load the settings.
    void Load();

    // Save the settings.
    void Save();

    // Show the settings window.
    void DrawUI(EditorNew* ed);

};