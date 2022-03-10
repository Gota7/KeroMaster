#pragma once

#include "raylib.h"
#include <imgui.h>
#include <string>
#include <string.h>
#include "../px/tileset.h"
#include "editor.h"
#include "selection.h"

using namespace std;

struct Editor;

struct TilesetEditor
{
    Editor* ed; // Main editor.
    bool open = false; // Window is open.
    string name; // Tileset name.
    RenderTexture2D target; // Where to draw the texture.
    RenderTexture2D finalTarget; // After transforming the original target (thanks OpenGL coordinates).
    float tileSize; // How big each tile is.
    Selection selection; // Current tile selection.
    int selectedTile; // The selected tile ID.
    bool focused; // If the tileset editor is focused.
    ImVec2 imgPos; // Position of the displayed tileset image.
    float imgSizeX; // How wide the image will be displayed.
    float imgSizeY; // How tall the image will be displayed.
    bool viewAttr = false; // If to view the tileset attributes.
    bool allowLayer0 = false; // Can paint this tileset on layer 0.
    bool allowLayer1 = false; // Can paint this tileset on layer 1.
    bool allowLayer2 = false; // Can paint this tileset on layer 2.
    int currLayer = -1; // Current layer to paint on.

    // Create a tileset editor to look at a tileset.
    TilesetEditor(Editor* ed, string name, float tileSize);

    // Draw the displayed tileset image.
    void Draw();
    
    // Draw the user interface.
    void DrawUI();

    // Update logic for the editor.
    void Update();

    // Close the editor.
    void Close();

};