#pragma once

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

    TilesetEditor(Editor* ed, string name);
    void Draw();
    void DrawUI();
    void Update();
    void Close();
};