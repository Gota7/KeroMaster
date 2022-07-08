#pragma once

#include "raylib.h"
#include <imgui.h>
#include <string>

struct Editor;

// Image viewer.
struct ImageViewer
{
    Editor* ed; // Main editor.
    bool open = false; // Window is open.
    std::string path; // Image path.
    std::string name; // Image name.
    Texture2D tex; // Actual image texture.
    ImVec2 imgPos; // Position of the displayed tileset image.
    float imgSizeX; // How wide the image will be displayed.
    float imgSizeY; // How tall the image will be displayed.

    // Create an image viewer.
    ImageViewer(Editor* ed, std::string path);

    // Draw the user interface.
    void DrawUI();

    // Close the viewer.
    void Close();

};