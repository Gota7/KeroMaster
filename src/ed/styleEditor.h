#pragma once

#include <string>

struct Editor;

struct StyleEditor
{
    Editor* ed; // Parent editor.
    bool open = false; // Style editor is open.
    char** themes = nullptr; // Themes for ImGui to select.
    int numThemes; // Number of themes.
    int currTheme = 0; // Current theme.
    bool scanDirs = true; // Scan for directories in the next draw.

    // Constructor. Does not open the editor by default.
    StyleEditor(Editor* ed);

    // Scan for themes.
    void ScanForThemes();

    // Draw UI.
    void DrawUI();

};