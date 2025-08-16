#pragma once

#include "tools/tools.h"
#include "../types.h"
#include <string>

struct Editor;

// Editor settings.
struct Settings
{
    u16 versionMajor = KM_VERSION_MAJOR; // Major version.
    u16 versionMinor = KM_VERSION_MINOR; // Minor version.
    u16 versionRevision = KM_VERSION_PATCH; // Revision version.
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
    std::string entityDataPath = "object_data/all.xml"; // Path for entity data.
    std::string scriptInfoPath = "object_data/scriptInfo.xml"; // Path for script info.
    std::string alwaysLoadedTilesetsPath = "object_data/alwaysLoaded.txt"; // Path for always loaded tilesets.
    std::string defaultCutscenePath = "object_data/default.pxeve"; // Path for the default cutscene.
    std::string tileAttributePath = "object_data/attribute.png"; // Path for tile attributes.
    std::string unitTypePath = "object_data/unittype.png"; // Path for the unit type image.
    std::string unitTypeListPath = "object_data/unittype.txt"; // Path for the unit type listing.
    std::string themesPath = "themes"; // Path for the theme folder.

    // Load the settings.
    void Load();

    // Save the settings.
    void Save();

    // Show the settings window.
    void DrawUI(Editor* ed);

    // If a selected path is valid.
    bool IsRscPathValid(std::string path);

};