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
    Editor* ed;
    bool open = false;
    string name;
    RenderTexture2D target;
    int currTile = -1;
    int selectionWidth;
    int selectionHeight;
    ImVec2 imgPos;
    float imgSize;
    bool viewAttr = false;
    bool selectingTiles = false;
    int startMouseX;
    int startMouseY;

    TilesetEditor(Editor* ed, string name);
    void Draw();
    void DrawUI();
    void Update();
    void CalcTiles();
    void Close();
};