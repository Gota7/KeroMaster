#pragma once

#include "attributeEditor.h"
#include "entityDisplay.h"
#include "entityEditor.h"
#include "levelEditor.h"
#include "musicPlayer.h"
#include "profileEditor.h"
#include "undoStack.h"
#include "selection.h"
#include "settings.h"
#include "scriptEditor.h"
#include "style.h"
#include "styleEditor.h"
#include "tilesetEditor.h"
#include "toolbar.h"
#include "tools/tools.h"
#include "../px/profile.h"
#include "../px/pxmap.h"
#include "../px/tileset.h"
#include "../rlImGui/focusData.h"
#include "raylib.h"

// The main editor.
struct Editor
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
    ProfileEditor profileEditor = ProfileEditor(this); // Profile editor.
    StyleEditor styleEditor = StyleEditor(this); // Style editor.
    MusicPlayer musicPlayer = MusicPlayer(this); // Music player.
    std::vector<TilesetEditor> tilesetEditors = std::vector<TilesetEditor>(); // Tileset editors.
    std::vector<AttributeEditor> attrEditors = std::vector<AttributeEditor>(); // Attribute editors.
    std::vector<ScriptEditor> scriptEditors = std::vector<ScriptEditor>(); // Script editors.
    Selection tilesToPaint; // Tiles to paint to the canvas.
    Map map; // Currently loaded pxmap.
    Profile profile; // Player profile.
    std::map<std::string, Tileset> tilesets; // Loaded tilesets.
    Camera2D cam; // Camera for viewing the map.
    UndoStack undoStack; // Undo stack for undo/redo actions.
    bool quit = false; // If to quit the editor.
    float oldMouseX; // Old mouse X position.
    float oldMouseY; // Old mouse Y position.
    float mouseX; // Mouse X position.
    float mouseY; // Mouse Y position.
    Toolbar toolbar = Toolbar(this); // Toolbar for widgets.
    Tools tools; // Editor tool widgets.
    ToolItem currTool = ToolItem::Hand; // Current tool.
    int currentLayer = (int)MapLayer::FG; // Current layer to edit.
    int placeEntityId = 0; // The entity ID to place if an entity is being placed.
    bool isPlacingEntity = false; // If an entity is being placed.
    bool doingEntityMove = false; // If an entity is moving.

    // New level selection vars.
    std::string newFileName; // Name for the new file.
    bool closeNewLevel; // Close the new level popup.
    bool isNew; // If the level to select is new.

    // Level file select vars.
    int numLevelFiles; // How many level files there are to display.
    char** levelFiles = nullptr; // Actual level file names.

    // Zoom constants.
    static constexpr float MIN_ZOOM = 0.25f;
    static constexpr float MAX_ZOOM = 10.0f;

    // Initialize the editor.
    void Init();

    // Main editor execution loop.
    int EditorLoop();

    // Draw the main background.
    void Draw();

    // Draw tile place preview.
    void DrawTilePlacePreview();

    // Draw entity place preview.
    void DrawEntityPlacePreview();

    // Draw the main UI.
    void DrawUI();

    // Main update loop.
    void Update();

    // Do the fade effect.
    void FadeEffect();

    // Initialize the editor by loading everything needed.
    void InitEditor();

    // Init sub-editors.
    void InitSubeditors();

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
    void OpenTileset(std::string tilesetName, float tileSize);

    // Remove selections from all other tileset viewers.
    void RemoveAllOtherTilesetViewerSelections(TilesetEditor* exclude);

    // Resize all tileset editors.
    void ResizeAllTilesetViewers(std::string name);

    // Open an attribute editor.
    void OpenAttr(std::string name, float tileSize);

    // Open a script.
    void OpenScript(std::string scriptName);

    // Move the camera in the X direction.
    void MoveCamX(float amount, bool relative = true);

    // Move the camera in the Y direction.
    void MoveCamY(float amount, bool relative = true);

    // Get how much to speed the zoom by.
    float GetZoomSpeed();

    // Zoom the camera in an out from a certain point with an amount.
    void Zoom(Vector2 origin, float amount, bool relative);

    // Get the tile at the X position with the current mouse and layer. -1 is out of bounds.
    int GetTileX(s8 layer = -1);

    // Get the tile at the Y position with the current mouse and layer. -1 is out of bounds.
    int GetTileY(s8 layer = -1);

    // Check for default tool applications.
    void DoDefaultToolRoutine();

    // Check for default scrolling with shortcuts.
    void CheckScroll();

    // Check for default zooming with mouse wheel.
    void CheckZoom();

    // Check for entity deleting.
    void CheckEntityDelete();

};