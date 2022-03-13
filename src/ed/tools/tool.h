#pragma once

#include "../../rlImGui/focusData.h"
#include <vector>

struct Editor;

// Tool item.
enum class ToolItem : int {
    Hand,
    TileBrush,
    Eraser,
    EntityHand,
    CurrentTool
};

// Tool that can be used by the editor.
struct Tool
{
    bool toolActive = false; // If the tool is activated.

    // Tool is selected, do its initial routine.
    virtual void Init(Editor* ed) {}

    // Activate the tool on a position.
    virtual void Activate(Editor* ed, Vector2 pos1 = { 0, 0 }, Vector2 pos2 = { 0, 0 }) {}

    // Update the tool.
    virtual void Update(Editor* ed) {}

    // Do things the editor needs to close up on.
    virtual void Close(Editor* ed) {}

    // Set the tool to be active depending on the mouse buttons.
    void SetActive(Editor* ed, std::vector<int>& buttons);

};