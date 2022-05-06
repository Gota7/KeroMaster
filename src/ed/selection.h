#pragma once

#include <climits>
#include <cmath>

// Selection structure.
struct Selection 
{
    bool validSelection = false; // If the selection is valid.
    int x = -1; // Starting x tile of the selection.
    int y = -1; // Starting y tile of the selection.
    int width = 0; // How wide the selection is in tiles.
    int height = 0; // How high the selection is in tiles.
    bool isSelecting = false; // If the mouse is currently down and selecting tiles.
    int startSelectTileX; // Starting x tile when selecting.
    int startSelectTileY; // Starting y tile when selecting.
    int layer; // Optional layer field to fill in when selecting.

    // Remove the current selection.
    void ClearSelection();

    // Correct a tile to be in the proper place. The maxes are not inclusive.
    static void CorrectTile(int* tileX, int* tileY, int minTileX, int minTileY, int maxTileX, int maxTileY);

    // Fix the selection to be within given constraints. The maxes are not inclusive.
    void MakeSelectionValid(int minTileX, int minTileY, int maxTileX, int maxTileY);

    // Get the tile the mouse is in.
    static void GetTileAtPos(float tileSize, int* tileXOut, int* tileYOut, int xPos, int yPos, float xOff = 0, float yOff = 0);

    // If the mouse is in the range of the canvas.
    static bool MouseInCanvas(float xOff = 0, float yOff = 0, float canvasXMin = NAN, float canvasYMin = NAN, float canvasXMax = NAN, float canvasYMax = NAN);

    // The selection is just an area with one click.
    void OneClickSelection(float tileSize, int selectionButton, bool focused, int minTileX = INT_MIN, int minTileY = INT_MIN, int maxTileX = INT_MAX, int maxTileY = INT_MAX, float xOff = 0, float yOff = 0, float canvasXMin = NAN, float canvasYMin = NAN, float canvasXMax = NAN, float canvasYMax = NAN);

    // Update the selection process given the offsets of the canvas. The mouse can also optionally be within a specified canvas to start selecting.
    void Update(float tileSize, int selectionButton, bool focused, int minTileX = INT_MIN, int minTileY = INT_MIN, int maxTileX = INT_MAX, int maxTileY = INT_MAX, float xOff = 0, float yOff = 0, float canvasXMin = NAN, float canvasYMin = NAN, float canvasXMax = NAN, float canvasYMax = NAN);

};