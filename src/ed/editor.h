#pragma once

#include <map>
#include "raylib.h"
#include "../px/pxmap.h"
#include "tilesetEditor.h"
#include "rlImGui/focusData.h"

using namespace std;

const float MAP_SIZE = 5;

struct TilesetEditor;
struct Editor
{
    Map map;
    string mapName = "";
    Camera2D cam;
    static std::map<string, Tileset> tilesets;
    static std::map<u8, EntityDisplay> entities;
    string rsc = "";
    bool enabled = false;
    Vector2 origin;
    float scrollSpeed = 7.5;
    float zoomSpeed = .1f;
    float oldMouseX;
    float oldMouseY;
    float mouseX;
    float mouseY;
    s32 tileMouseX;
    s32 tileMouseY;
    bool showPlayArea = true;
    bool showGrid = true;
    bool viewLayers[3] = { true, true, true };
    bool viewEntityBoxes = true;
    bool viewEntities = true;
    bool viewTileAttributes = false;
    bool inPan = false;
    bool openTilesetsOnLoad = false;
    FocusData focus;
    Entity* editingEntity = NULL;
    vector<TilesetEditor> tilesetEditors = vector<TilesetEditor>();

    void SetPath(string rsc);
    void LoadEnemies(string xml);
    void LoadTileset(string tilesetName);
    void LoadFixedTilesets();
    void LoadLevel(string name);
    void UnloadLevel();
    void MoveCamX(float amount, bool relative = true);
    void MoveCamY(float amount, bool relative = true);
    void Draw();
    void DrawUI();
    void DrawMainMenu();
    void DrawLevelEditor();
    void DrawEntityEditor();
    void Update();
    void CheckPan();
    void CheckScroll();
    void CheckZoom();
};

int cmpstr(const void* a, const void* b);
