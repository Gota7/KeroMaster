#pragma once

#include "raylib.h"
#include <imgui.h>
#include <string>
#include <string.h>
#include "../px/tileset.h"
#include "editor.h"

using namespace std;

struct Editor;

struct TilesetEditor
{
    Editor* ed; // Main editor.
    bool open = false; // Window is open.
    string name; // Tileset name.
    RenderTexture2D target; // Where to draw the texture.
    int currTile = -1;
    int selectionWidth;
    int selectionHeight;
    ImVec2 imgPos;
    float imgSizeX;
    float imgSizeY;
    bool viewAttr = false;
    bool selectingTiles = false;
    int startMouseX;
    int startMouseY;
    bool allowLayer0 = false;
    bool allowLayer1 = false;
    bool allowLayer2 = false;
    int currLayer = -1;

    TilesetEditor(Editor* ed, string name);
    void Draw();
    void DrawUI();
    void Update();
    void CalcTiles();
    void Close();
};