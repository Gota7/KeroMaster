#pragma once

#include <map>
#include "raylib.h"
#include "../px/pxmap.h"
#include "tilesetEditor.h"
#include "attributeEditor.h"
#include "scriptEditor.h"
#include "rlImGui/focusData.h"

constexpr float MAP_SIZE = 2.0f;

struct AttributeEditor;
struct TilesetEditor;
struct ScriptEditor;

enum class EditorTool {
    Hand,
    TileBrush,
    Eraser,
    EntityHand
};

struct Editor
{
    Map map;
    string mapName = "";
    Camera2D cam = {0};
    static std::map<string, Tileset> tilesets;
    static std::map<u8, EntityDisplay> entities;
    string rsc = "";
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
    u16 currentTile = 0;
    EditorTool currentTool = EditorTool::Hand;
    FocusData focus;
    Entity* editingEntity = NULL;
    std::vector<TilesetEditor> tilesetEditors = std::vector<TilesetEditor>();
    std::vector<AttributeEditor> attrEditors = std::vector<AttributeEditor>();
    std::vector<ScriptEditor> scriptEditors = std::vector<ScriptEditor>();
    char* entityListing;
    bool resizeWarning = false;
    ShiftDirection resizeMode = ShiftDirection::Resize;
    u8 resizeMapLayer = 0;
    int placeEntityId = 0;
    bool isPlacingEntity = false;

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
    void DrawMainMenu();
    void DrawLevelEditor();
    void DrawEntityEditor();
    void DrawPalette();
    void DrawToolbar();
    void DrawProfileEditor();
    void OpenTileset(std::string name);
    void OpenAttrEditor(std::string name);
    void OpenScript(std::string name);
    void ResizeAllTilesetViewers(std::string name);
    void Update();
    void CheckPan();
    void CheckScroll();
    void CheckZoom();
    void CheckEdit();
    void CheckEntity();
};

int cmpstr(const void* a, const void* b);
