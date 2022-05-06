#pragma once

#include "entityDisplay.h"
#include "entityEditor.h"
#include "levelEditor.h"
#include "undoStack.h"
#include "settingsNew.h"
#include "style.h"
#include "tools/tools.h"
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
    bool quit = false; // If to quit the editor.
    float oldMouseX; // Old mouse X position.
    float oldMouseY; // Old mouse Y position.
    float mouseX; // Mouse X position.
    float mouseY; // Mouse Y position.
    Tools tools; // Editor tool widgets.
    ToolItem currTool = ToolItem::Hand; // Current tool.

    // New level selection vars.
    std::string newFileName; // Name for the new file.
    bool closeNewLevel; // Close the new level popup.
    bool isNew; // If the level to select is new.

    // Level file select vars.
    int numLevelFiles; // How many level files there are to display.
    char** levelFiles = nullptr; // Actual level file names.

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

    // Select a level name.
    void LevelNameSelect(bool saveAs);

    // Open a level name.
    void LevelNameOpen();

    // Draw open level UI.
    void DrawSelectLevelUI();

    // Draw the open level UI.
    void DrawOpenLevelUI();

    // Draw about popup.
    void DrawAboutPopup();

    // Open a level.
    void OpenLevel(std::string level);

    // Save the level.
    void SaveLevel();

    // Close the level.
    void CloseLevel();

    // Quit the editor.
    void Quit();

    // Undo the last action.
    void Undo();

    // Redo the last action.
    void Redo();

    // Toggle fullscreen.
    void DoToggleFullscreen();

    // Open about popup.
    void OpenAboutPopup();

    // Open a tileset.
    void OpenTileset(std::string tilesetName);

    // Open a script.
    void OpenScript(std::string scriptName);

    // Move the camera in the X direction.
    void MoveCamX(float amount, bool relative = true);

    // Move the camera in the Y direction.
    void MoveCamY(float amount, bool relative = true);

    // Get the tile at the X position with the current mouse and layer. -1 is out of bounds.
    int GetTileX(s8 layer = -1);

    // Get the tile at the Y position with the current mouse and layer. -1 is out of bounds.
    int GetTileY(s8 layer = -1);

};