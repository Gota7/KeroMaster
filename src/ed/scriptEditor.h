#pragma once

#include <string>
#include "imgui.h"

struct Editor;

// Argument types for a script command.
enum ScriptArgType
{
    ARG_TYPE_NUMBER,
    ARG_TYPE_STRING
};

// A command argument.
struct ScriptHelpArg
{
    ScriptArgType type; // Command type.
    std::string description; // Command description.
};

// Data for a single command.
struct ScriptHelpData
{
    std::string name; // Command name.
    std::string description; // Command description.
    ScriptHelpArg* args; // Command arguments.
    int numArgs = 0; // Number of arguments.
};

struct ScriptEditor
{
    Editor* ed; // Main editor.
    std::string name; // Name of the script.
    ImVector<char> buf; // Editing buffer.
    static bool loadedGuide; // If the guide has been loaded or not.
    bool open = false; // If the window is open.
    static ScriptHelpData* commands; // Loaded script help commands.
    static int numCommands; // Number of script help commands.
    char search[5] = { 0, 0, 0, 0, 0 }; // Characters used to search through help.
    bool isShiftJIS; // If the current file being edited is shift JIS or UTF8.

    // Create a new script editor with a script to edit.
    ScriptEditor(Editor* ed, std::string scriptName);

    // Load the command help XML.
    void LoadXML();

    // Load the script to edit.
    void LoadScript();

    // Save the script being edited.
    void SaveScript();

    // Draw the editor UI.
    void DrawUI();

    // Close the script editor.
    void Close();

};

// Needed for editing the script text.
static int ResizeCallback(ImGuiInputTextCallbackData* data);