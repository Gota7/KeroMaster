#pragma once

#include "../../rlImGui/focusData.h"
#include <vector>

struct EditorNew;

// Tool item.
enum class ToolItem : int {
    Hand,
    TileBrush,
    Eraser,
    EntityHand,
    CurrentTool,
    NumTools = CurrentTool
};

// Init, update, or close function.
typedef void (*ToolBasicFunction)(EditorNew* ed);

// Tool activate function.
typedef void (*ToolActivateFunction)(EditorNew* ed, Vector2 pos1, Vector2 pos2);

// Tool that can be used by the editor.
struct Tools
{
    bool toolActive[(int)ToolItem::NumTools]; // If the tool is activated.
    static const char* toolNames[(int)ToolItem::NumTools]; // Names of the tools.
    static const ToolBasicFunction initFunctions[(int)ToolItem::NumTools]; // Tool init functions.
    static const ToolActivateFunction activateFunctions[(int)ToolItem::NumTools]; // Tool activate functions.
    static const ToolBasicFunction updateFunctions[(int)ToolItem::NumTools]; // Tool update functions.
    static const ToolBasicFunction closeFunctions[(int)ToolItem::NumTools]; // Tool close functions.

    // Tool is selected, do its initial routine.
    void Init(EditorNew* ed, int toolItem);

    // Activate the tool on a position.
    void Activate(EditorNew* ed, int toolItem, Vector2 pos1 = { 0, 0 }, Vector2 pos2 = { 0, 0 });

    // Update the tool.
    void Update(EditorNew* ed, int toolItem);

    // Do things the editor needs to close up on.
    void Close(EditorNew* ed, int toolItem);

    // Set the tool to be active depending on the mouse buttons.
    void SetActive(EditorNew* ed, int toolItem, std::vector<int>& buttons);

    // Run general tool update function.
    void Update(EditorNew* ed);

};