#pragma once

struct Editor;

// For music playback.
struct MusicPlayer
{
    Editor* ed; // Main editor program.
    bool open = false; // If the player is open.

    // Create a new music player.
    MusicPlayer(Editor* ed);

    // Draw the interface.
    void DrawUI();

};