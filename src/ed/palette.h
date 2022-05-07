#pragma once

#include "selection.h"

#include "raylib.h"
#include <imgui.h>
#include <string>

struct Editor;

// The palette to display tiles.
struct Palette
{
    Editor* ed; // Editor for the palette to use.
    RenderTexture2D target; // Where to draw the texture.
    RenderTexture2D finalTarget; // After transforming the original target (thanks OpenGL coordinates).
    bool viewAttr = false; // If to view the tileset attributes.
    std::string name = ""; // Tileset name.
    float tileSize; // How big each tile is.
    Selection selection; // Current tile selection.
    int selectedTile; // The selected tile ID.
    bool focused; // If the tileset editor is focused.
    ImVec2 imgPos; // Position of the displayed tileset image.
    float imgSizeX; // How wide the image will be displayed.
    float imgSizeY; // How tall the image will be displayed.

    // New palette.
    Palette(Editor* ed);

    // Draw the displayed tileset image.
    void Draw();

    // Draw the user interface.
    void DrawUI();

    // Update logic for the palette.
    void Update();

    // Change the tileset loaded.
    void ChangeTileset(std::string newTileset);

};