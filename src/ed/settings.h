#pragma once

#include "tools/tools.h"
#include "../types.h"
#include <string>

struct Editor;

// Editor settings.
struct Settings
{
    u16 versionMajor = 0; // Major version.
    u16 versionMinor = 1; // Minor version.
    u16 versionRevision = 0; // Revision version.
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
    bool softShow = false; // If to show the settings window but its controlled and not from a problem.
    bool showPlayArea = true; // If to show bounds of each map.
    bool showGrid = false; // Show the grid.
    bool viewLayers[3] = { true, true, true }; // What layers can be seen.
    bool viewEntityImages = true; // If to show entity images, if not, show the images from unit type.
    bool viewEntityBoxes = true; // Show the boxes for entities.
    bool viewEntities = true; // Show entities.
    bool viewTileAttributes = false; // If to view tile attributes.
    bool openTilesetsOnLoad = false; // Open tilesets on load.
    int maxUndoDepth = 1000; // Maximum undo depth.

    // Load the settings.
    void Load();

    // Save the settings.
    void Save();

    // Show the settings window.
    void DrawUI(Editor* ed);

};