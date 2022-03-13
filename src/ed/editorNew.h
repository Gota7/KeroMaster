#pragma once

#include "entityDisplay.h"
#include "entityEditor.h"
#include "levelEditor.h"
#include "undoStack.h"
#include "settingsNew.h"
#include "style.h"
#include "../px/pxmap.h"
#include "../px/tileset.h"
#include "../rlImGui/focusData.h"
#include "raylib.h"

// The main editor.
struct EditorNew
{
    SettingsNew settings; // Editor settings.
    EditorStyle style; // Style for the editor.
    Color fadeColor = { 255, 0, 0, 255 }; // Background color.
    Image icon; // Editor icon.
    bool enabled = false; // If the editor is enabled and ready to edit.
    bool doFullscreen = false; // If the editor is in fullscreen mode.
    FocusData focus; // For focusing subwindows.
    std::string rsc = ""; // Resource path.
    std::string level = ""; // Open level.
    EntityEditor entityEditor = EntityEditor(this); // Entity editor.
    LevelEditor levelEditor = LevelEditor(this); // Level editor.
    Map map; // Currently loaded pxmap.
    std::map<std::string, Tileset> tilesets; // Loaded tilesets.
    Camera2D cam; // Camera for viewing the map.
    UndoStack undoStack; // Undo stack for undo/redo actions.

    // Initialize the editor.
    void Init();

    // Main editor execution loop.
    int EditorLoop();

    // Draw the main background.
    void Draw();

    // Draw the main UI.
    void DrawUI();
    
    // Main update loop.
    void Update();

    // Do the fade effect.
    void FadeEffect();

    // Initialize the editor by loading everything needed.
    void InitEditor();

    // Load a tileset.
    void LoadTileset(std::string tilesetName);

    // Unload a tileset.
    void UnloadTileset(std::string tilesetName);

    // Load the fixed tilesets.
    void LoadFixedTilesets();

    // Load a level from the current level name.
    void LoadLevel();

    // Unload the current level.
    void UnloadLevel();

    // Draw the grid.
    void DrawGrid();

    // Detect if a keyboard shortcut is down.
    bool KeyboardShortcut(bool control, bool alt, bool shift, int key);

    // Draw the main menu at the top of the screen.
    void DrawMainMenu();

    // Open a tileset.
    void OpenTileset(std::string tilesetName);

    // Open a script.
    void OpenScript(std::string scriptName);

};