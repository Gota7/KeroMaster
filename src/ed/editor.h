#pragma once

#include <map>
#include "raylib.h"
#include "../px/pxmap.h"

using namespace std;

const float MAP_SIZE = 5;

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

    void SetPath(string rsc);
    void LoadEnemies(string xml);
    void LoadTileset(string tilesetName);
    void LoadFixedTilesets();
    void LoadLevel(string name);
    void Draw();
    void DrawUI();
    void Update();
};
