#pragma once

#include "../types.h"

struct EditorNew;
struct Profile;

// For editing save data.
struct ProfileEditor
{
    EditorNew* ed; // Base editor.
    bool open = false; // If this is open.
    Profile* profile; // The profile save to edit.

    // Create a new profile editor.
    ProfileEditor(EditorNew* ed);

    // Draw the main interface.
    void DrawUI();

    // Draw general settings.
    void DrawGeneralSettings(int itemWidth);

    // Draw screen settings.
    void DrawScreenSettings(int itemWidth);

    // Controller settings.
    void DrawControlSettings(int itemWidth);

    // Draw a control option.
    void DrawControlOption(int itemWidth, const char* name, u8* field, const char* desc);

    // Draw a control option for the joystick.
    void DrawControlOptionJoystick(int itemWidth, const char* name, u8* field, const char* desc);

    // Game profile viewing.
    void DrawGameProfile(int itemWidth);

    // Save info viewing.
    void DrawSaveInfo(int itemWidth, int save);

};