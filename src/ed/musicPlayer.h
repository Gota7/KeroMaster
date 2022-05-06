#pragma once

struct EditorNew;

// For music playback.
struct MusicPlayer
{
    EditorNew* ed; // Main editor program.
    bool open; // If the player is open.

    // Create a new music player.
    MusicPlayer(EditorNew* ed);

    // Draw the interface.
    void DrawUI();

};