#pragma once

#include <map>
#include <string>
#include "attributeEditor.h"
#include "scriptEditor.h"
#include "settings.h"
#include "tilesetEditor.h"
#include "../px/pxmap.h"
#include "../px/profile.h"
#include "../px/tileset.h"
#include "../rlImGui/focusData.h"

// Scale the map properly.
constexpr float MAP_SIZE = Tileset::EDITOR_TILE_SIZE / Tileset::MAP_TILE_SIZE;

struct AttributeEditor;
struct EntityEditor;
struct LevelEditor;
struct MusicPlayer;
struct ProfileEditor;
struct ScriptEditor;
struct Settings;
struct StyleEditor;
struct TilesetEditor;
struct UndoStack;

struct Tool;
//struct HandTool;

enum class EditorTool : int {
    Hand,
    TileBrush,
    Eraser,
    EntityHand,
    CurrentTool
};

struct Editor
{
    static Settings settings;
    Map map;
    Profile profile;
    std::string mapName = "";
    Camera2D cam = {0};
    static std::map<std::string, Tileset> tilesets;
    static std::map<u8, EntityDisplay> entities;
    static Color fadeColor;
    static double timer;
    static bool doFullscreen;
    std::string rsc = "";
    bool enabled = false;
    Vector2 origin;
    float scrollSpeed = 7.5;
    float zoomSpeed = .05f;
    float oldMouseX;
    float oldMouseY;
    float mouseX;
    float mouseY;
    s32 tileMouseX;
    s32 tileMouseY;
    bool showPlayArea = true;
    bool showGrid = false;
    bool viewLayers[3] = { true, true, true };
    bool viewEntityBoxes = true;
    bool viewEntities = true;
    bool viewTileAttributes = false;
    bool inPan = false;
    bool dragLeft = false;
    bool dragRight = false;
    bool openTilesetsOnLoad = false;
    bool showPaletteAttributes = false;;
    u8 currentLayer = 0;
    bool isSelecting = false;
    int currentTile = -1;
    int selectionStartX;
    int selectionStartY;
    int selectionWidth = 1;
    int selectionHeight = 1;
    EditorTool currentTool = EditorTool::Hand;
    FocusData focus;
    std::vector<TilesetEditor> tilesetEditors = std::vector<TilesetEditor>();
    std::vector<AttributeEditor> attrEditors = std::vector<AttributeEditor>();
    std::vector<ScriptEditor> scriptEditors = std::vector<ScriptEditor>();
    int placeEntityId = 0;
    bool isPlacingEntity = false;
    bool doingEntityMove = false;
    u16 backupEntityX;
    u16 backupEntityY;
    EntityEditor* entityEditor = nullptr;
    LevelEditor* levelEditor = nullptr;
    MusicPlayer* musicPlayer = nullptr;
    ProfileEditor* profileEditor = nullptr;
    StyleEditor* styleEditor = nullptr;
    TilesetEditor* editingTileset = nullptr;
    bool showHelp = false;
    bool helpModal = false;
    UndoStack* undoStack;

    // Tools.
    //static std::vector<Tool> tools;
    Tool* currTool;

    void Init();
    void SetPath(std::string rsc);
    void LoadEnemies(std::string xml);
    void LoadTileset(std::string tilesetName);
    void LoadFixedTilesets();
    void LoadLevel(std::string name);
    void UnloadLevel();
    void MoveCamX(float amount, bool relative = true);
    void MoveCamY(float amount, bool relative = true);
    int GetTileX(s8 layer = -1);
    int GetTileY(s8 layer = -1);
    void Draw();
    void DrawUI();
    void DrawMainMenu(bool startup = false);
    void DrawPalette();
    void DrawToolbar();
    void OpenTileset(std::string name);
    void OpenAttrEditor(std::string name);
    void OpenScript(std::string name);
    void ResizeAllTilesetViewers(std::string name);
    void RemoveAllOtherTilesetViewerSelections(TilesetEditor* exclude);
    void Update();
    void CheckPan();
    void CheckScroll();
    void CheckZoom();
    void CheckEdit();
    void CheckEntity();
    void UpdateFadeColor();
};

int cmpstr(const void* a, const void* b);
