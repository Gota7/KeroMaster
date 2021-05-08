#pragma once
#include "editor.h"
#include <string>

struct Editor;

struct AttributeEditor
{
    Editor* ed;
    RenderTexture2D target;
    string name;
    bool open = false;
    ImVec2 imgPos;
    float imgSizeX;
    float imgSizeY;
    int mode = 0;
    int attrTile = 0;
    bool isFocused;

    AttributeEditor(Editor* ed, std::string name);
    void Draw();
    void DrawUI();
    void Update();
    void Close();
};