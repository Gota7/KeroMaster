#pragma once

#include "selection.h"
#include "raylib.h"
#include <imgui.h>
#include <string>

struct Editor;

struct AttributeEditor
{
    Editor* ed; // Referenced editor.
    RenderTexture2D target; // Target to draw displayed image to.
    RenderTexture2D finalTarget; // Final flipped image hack.
    std::string name; // Tileset name.
    float tileSize; // Tile size.
    bool open = false; // Window is open.
    ImVec2 imgPos; // Position of the image to display.
    float imgSizeX; // Image width.
    float imgSizeY; // Image height.
    int mode = 0; // If to paint or release.
    Selection attrSelection; // Attribute tile selection.
    Selection tileSelection; // Tile to paint on selection.
    int attrTile = 0; // Attribute tile index.
    bool isFocused; // If the attribute editor is focused.

    // Create an attribute editor given an editor and a tileset name.
    AttributeEditor(Editor* ed, std::string name, float tileSize);

    // Draw the tile graphics.
    void Draw();

    // Draw the ImGui UI.
    void DrawUI();

    // Update variables.
    void Update();

    // Close the editor.
    void Close();

};