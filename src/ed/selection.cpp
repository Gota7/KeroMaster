#include "selection.h"
#include "raylib.h"
#include <algorithm>
#include <stdio.h>

void Selection::ClearSelection()
{
    validSelection = false;
    x = -1;
    y = -1;
    width = 0;
    height = 0;
    isSelecting = false;
}

void Selection::CorrectTile(int* tileX, int* tileY, int minTileX, int minTileY, int maxTileX, int maxTileY)
{
    if (*tileX < minTileX) *tileX = minTileX;
    if (*tileY < minTileY) *tileY = minTileY;
    if (*tileX >= maxTileX) *tileX = maxTileX - 1;
    if (*tileY >= maxTileY) *tileY = maxTileY - 1;
}

void Selection::MakeSelectionValid(int minTileX, int minTileY, int maxTileX, int maxTileY)
{
    if (x < minTileX) x = minTileX;
    if (y < minTileY) y = minTileY;
    if (x + width > maxTileX) width = maxTileX - x;
    if (y + height > maxTileY) height = maxTileY - y;
    validSelection = width > 0 && height > 0;
}

void Selection::GetTileAtPos(float tileSize, int* tileXOut, int* tileYOut, int xPos, int yPos, float xOff, float yOff)
{
    *tileXOut = (xPos - xOff) / tileSize;
    *tileYOut = (yPos - yOff) / tileSize;
}

bool Selection::MouseInCanvas(float xOff, float yOff, float canvasXMin, float canvasYMin, float canvasXMax, float canvasYMax)
{
    float mouseX = GetMouseX() - xOff;
    float mouseY = GetMouseY() - yOff;
    if (canvasXMin != NAN && mouseX < canvasXMin) return false;
    if (canvasXMax != NAN && mouseX > canvasXMax) return false;
    if (canvasYMin != NAN && mouseY < canvasYMin) return false;
    if (canvasYMax != NAN && mouseY > canvasYMax) return false;
    return true;
}

void Selection::OneClickSelection(float tileSize, int selectionButton, bool focused, int minTileX, int minTileY, int maxTileX, int maxTileY, float xOff, float yOff, float canvasXMin, float canvasYMin, float canvasXMax, float canvasYMax)
{
    if (MouseInCanvas(xOff, yOff, canvasXMin, canvasYMin, canvasXMax, canvasYMax) && IsMouseButtonDown(selectionButton) && focused)
    {
        GetTileAtPos(tileSize, &x, &y, GetMouseX(), GetMouseY(), xOff, yOff);
        width = 1;
        height = 1;
        isSelecting = false;
        MakeSelectionValid(minTileX, minTileY, maxTileX, maxTileY);
    }
}

void Selection::Update(float tileSize, int selectionButton, bool focused, int minTileX, int minTileY, int maxTileX, int maxTileY, float xOff, float yOff, float canvasXMin, float canvasYMin, float canvasXMax, float canvasYMax)
{

    // Initial click.
    if (MouseInCanvas(xOff, yOff, canvasXMin, canvasYMin, canvasXMax, canvasYMax) && IsMouseButtonPressed(selectionButton) && focused)
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        GetTileAtPos(tileSize, &startSelectTileX, &startSelectTileY, mouseX, mouseY, xOff, yOff);
        CorrectTile(&startSelectTileX, &startSelectTileY, minTileX, minTileY, maxTileX, maxTileY);
        isSelecting = true;
    }

    // Finished selection.
    else if (IsMouseButtonReleased(selectionButton) && isSelecting)
    {
        isSelecting = false;
    }

    // Update selection if selecting.
    if (isSelecting)
    {

        // Get the minimum tile position and calculate sizes.
        int tileX;
        int tileY;
        GetTileAtPos(tileSize, &tileX, &tileY, GetMouseX(), GetMouseY(), xOff, yOff);
        CorrectTile(&tileX, &tileY, minTileX, minTileY, maxTileX, maxTileY);
        x = std::min(tileX, startSelectTileX);
        y = std::min(tileY, startSelectTileY);
        width = abs(tileX - startSelectTileX) + 1;
        height = abs(tileY - startSelectTileY) + 1;

        // Make final selection valid.
        MakeSelectionValid(minTileX, minTileY, maxTileX, maxTileY);

    }

}