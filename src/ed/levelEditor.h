#pragma once

#include "../px/pxmap.h"

struct Editor;

// For editing a level.
struct LevelEditor
{
    Editor* ed; // Main editor.
    bool open = true; // If the level editor is open.
    bool resizeWarning = false; // If a resize is going to happen and a warning should be shown.
    ShiftDirection resizeMode = ShiftDirection::Resize; // Where to shift, or resize tiles.
    u8 resizeMapLayer = 0; // The layer to resize.

    // Create a new editor.
    LevelEditor(Editor* ed);

    // Draw the level editor interface.
    void DrawUI();

    // Draw tileset settings.
    void DrawTilesetSettings(int tilesetNum, int itemWidth);

    // Check if map can be shifted in a direction.
    void CanShiftInDirCheck(ShiftDirection dir, int layer);

    // Edit a level direction button.
    void EditLevelButton(int id);

    // Draw the map shifting/resizing warning when data would be lost. This also resizes if the user accepts.
    void DrawMapShiftResizeWarning(int itemWidth);

};