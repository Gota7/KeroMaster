#pragma once

#include "editor.h"
#include <string>

struct Editor;

struct AttributeEditor
{
    Editor* ed; // Referenced editor.
    RenderTexture2D target; // Target to draw displayed image to.
    string name; // Tileset name.
    bool open = false; // Window is open.
    ImVec2 imgPos; // Position of the image to display.
    float imgSizeX; // Image width.
    float imgSizeY; // Image height.
    int mode = 0; // If to paint or release.
    int attrTile = 0; // Attribute tile index.
    bool isFocused; // If the attribute editor is focused.

    // Create an attribute editor given an editor and a tileset name.
    AttributeEditor(Editor* ed, std::string name);

    // Draw the tile graphics.
    void Draw();

    // Draw the ImGui UI.
    void DrawUI();

    // Update variables.
    void Update();

    // Close the editor.
    void Close();
    
};