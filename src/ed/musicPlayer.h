#pragma once

#include "editor.h"
#include "../bgm/bgm.h"
#include <imgui.h>

struct Editor;

// For music playback.
struct MusicPlayer
{
    Editor* ed; // Main editor program.
    bool open; // If the player is open.

    // Create a new music player.
    MusicPlayer(Editor* ed);

    // Draw the interface.
    void DrawUI();

};