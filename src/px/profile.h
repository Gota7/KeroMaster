#pragma once

#include <string>
#include <vector>
#include <imgui.h>
#include "../types.h"
#include "../gbin/gfile.h"
#include "raylib.h"

// Kero blaster save.
struct ProfileSave
{

};

// Kero blaster profile.
struct Profile
{

    // Number of saves.
    static constexpr u8 NUM_SAVES = 3;
    static constexpr u8 NUM_GAME_FLAGS = 0x20;

    // Params.
    bool fullscreen = false;
    std::string region = "ENGLISH";
    s32 topLeftCorner[2] = { 720, 367 };
    s32 bottomRightCorner[2] = { 1200, 687 }; // Doesn't seem to matter?
    u8 screenSize = 1;
    u16 lastYear = 2021;
    u8 lastMonth = 5;
    u8 lastDay = 8;
    u8 lastHour = 21;
    u8 lastMinute = 12;
    u8 lastSecond = 16;
    u8 lastProfileSave = 0;
    u8 useJoystick = 0;
    u8 keyboardLeft = 0x25;
    u8 keyboardUp = 0x26;
    u8 keyboardRight = 0x27;
    u8 keyboardDown = 0x28;
    u8 keyboardJump = 0x5A;
    u8 keyboardShoot = 0x58;
    u8 keyboardWeaponSwitchLeft = 0x41;
    u8 keyboardWeaponSwitchRight = 0x53;
    u8 keyboardMenu = 0x1B;
    u8 joystickJump = 0;
    u8 joystickShoot = 1;
    u8 joystickWeaponSwitchLeft = 3;
    u8 joystickWeaponSwitchRight = 4;
    u8 joystickMenu = 2;
    u8 gameProfile[NUM_GAME_FLAGS];
    std::vector<ProfileSave> saves = std::vector<ProfileSave>();

    // If a file exists in the profile folder.
    bool FileExists(std::string rsc_k, std::string name);

    // Get a Gota's bin file from the profile directory. 
    GFile GetGFile(std::string rsc_k, std::string name);

    // Read all files.
    void ReadAll(std::string rsc_k);

    // Load the fullscreen file.
    void LoadFullscreen(std::string rsc_k);

    // Save the fullscreen file.
    void SaveFullscreen(std::string rsc_k);

    // Load the game profile.
    void LoadGameProfile(std::string rsc_k);

    // Save the game profile.
    void SaveGameProfile(std::string rsc_k);

    // Load the last controller interface mode used.
    void LoadControlInterfaceMode(std::string rsc_k);

    // Save the last controller interface mode used.
    void SaveControlInterfaceMode(std::string rsc_k);

    // Load the region file.
    void LoadRegion(std::string rsc_k);

    // Save the region file.
    void SaveRegion(std::string rsc_k);

    // Load the screen position.
    void LoadRect(std::string rsc_k);

    // Save the screen position.
    void SaveRect(std::string rsc_k);

    // Load the last profile used.
    void LoadLastIdx(std::string rsc_k);

    // Save the last profile used.
    void SaveLastIdx(std::string rsc_k);

    // Load a save file.
    void LoadProfileSave(std::string rsc_k, u8 num);

    // Save a save file.
    void SaveProfileSave(std::string rsc_k, u8 num);

    // Load the last screen size used.
    void LoadSize(std::string rsc_k);

    // Save the last screen size used.
    void SaveSize(std::string rsc_k);

    // Load the last time used.
    void LoadLastTime(std::string rsc_k);

    // Save the last time used.
    void SaveLastTime(std::string rsc_k);
    
    // Load the last key config used.
    void LoadKeyConfig(std::string rsc_k);

    // Save the last key config used.
    void SaveKeyConfig(std::string rsc_k);

};