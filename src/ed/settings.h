#pragma once

#include "raylib.h"
#include "editor.h"
#include "style.h"
#include <string>
#include <cstring>

struct Editor;
enum class EditorTool;
struct FocusData;

struct Settings
{
    std::string rscPath = "";
    std::string lastLevel = "";
    bool usePalette = true;
    EditorTool leftClick = (EditorTool)4;
    EditorTool rightClick = (EditorTool)0;
    EditorTool middleClick = (EditorTool)3;
    EditorStyle style;
    bool show = false;

    void Load();
    void Save();
    void ShowWindow(Editor* ed);
    bool ButtonUp(EditorTool tool);
    bool ButtonDown(EditorTool tool);
    bool ButtonPressed(EditorTool tool);
    bool ButtonReleased(EditorTool tool);
    bool ButtonClicked(EditorTool tool, bool repeat = false);
};