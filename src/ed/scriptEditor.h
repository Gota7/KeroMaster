#pragma once

#include "editor.h"
#include <string>

struct Editor;

enum ScriptArgType
{
    ARG_TYPE_NUMBER,
    ARG_TYPE_STRING
};

struct ScriptHelpArg
{
    ScriptArgType type;
    std::string description;
};

struct ScriptHelpData
{
    std::string name;
    std::string description;
    ScriptHelpArg* args;
    int numArgs = 0;
};

struct ScriptEditor
{
    Editor* ed;
    std::string name;
    ImVector<char> buf;
    static bool loadedGuide;
    bool open = false;
    ScriptHelpData* commands;
    int numCommands = 0;
    char search[5] = { 0, 0, 0, 0, 0 };

    ScriptEditor(Editor* ed, std::string scriptName);
    void LoadXML();
    void LoadScript();
    void SaveScript();
    void DrawUI();
    void Close();
};

static int ResizeCallback(ImGuiInputTextCallbackData* data);