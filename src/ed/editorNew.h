#pragma once

#include "settingsNew.h"
#include "style.h"
#include "raylib.h"

// The main editor.
struct EditorNew
{
    SettingsNew settings; // Editor settings.
    EditorStyle style; // Style for the editor.
    Color fadeColor = { 255, 0, 0, 255 }; // Background color.
    Image icon; // Editor icon.
    bool enabled = false; // If the editor is enabled and ready to edit.
    bool doFullscreen = false; // If the editor is in fullscreen mode.

    // Initialize the editor.
    void Init();

    // Main editor execution loop.
    int EditorLoop();

    // Draw the main background.
    void Draw();

    // Draw the main UI.
    void DrawUI();
    
    // Main update loop.
    void Update();

    // Do the fade effect.
    void FadeEffect();

};