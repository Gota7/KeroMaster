#pragma once

#include <string>
#include <vector>
#include <imgui.h>
#include "../types.h"
#include "../gbin/gfile.h"
#include "raylib.h"

/*
    00 - Header, 8
    08 - Save year, 1
    09 - Save month, 1
    0A - Save day, 1
    0B - Save hour, 1
    0C - Save minute, 1
    0D - Save second, 1
    0E - Zeros, 2
    10 - Profile icon, 4
    14 - Total playtime in frames for menu, 4
    18 - Display money, 4
    1C - Display lives, 2
    1E - Display hearts, 2
    20 - UNKNOWN, is 2 in my Omake save but 0 for rest, 2
    22 - Mission description, 82
    A4 - Items to display, 4[8]
    C4 - Total playtime in frames, 4
    C8 - Number of flag bytes, 4
    CC - Flags, 800
    8CC - Items in hand, 4[4]
    8DC - Currently held item, 1
    8DD - Weapons in hand, Weapons[8]
    8FD - Number of lives, 2
    8FF - Money, 4
    903 - UNKNOWN (0 or 2?), 2
    905 - UNKNOWN (D, 4, 28, etc.), 2
    907 - Current HP, 2
    909 - Total HP, 2
    90B - UNKNOWN (always 1?), 1
    90C - On gameover level, 10
    940 - Current level, C
    94C - UNKNOWN (always 72?), 2
    UNK
    965 - X position of some kind, 4
    969 - Y position of some kind, 4
    96D - UNKNOWN (always 0?), 2
    96F - Player facing right, 4
    UNK
    974 - Checkpoint stage, 10
    984 - Shop teleporter string parameter, ?
    UNK

    struct Weapons {
        00 - Item ID, 1
        01 - Item level, 1
        02 - Zeroes, 2 (Last byte may be set sometimes, but idk what to?)
    }
*/

// A weapon.
struct Weapon
{
    u8 itemId = 0;
    u8 itemLevel = 0;
    u8 unk1 = 0;
    u8 unk2 = 0;
};

// Kero blaster save.
struct ProfileSave
{
    u8 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u32 profileIcon;
    u32 displayPlaytime;
    u32 displayMoney;
    u16 displayLives;
    u16 displayHearts;
    u16 unk1;
    std::string missionDescription;
    u32 displayItems[8];
    u32 playtime;
    u8 flags[0x800];
    u32 itemsInHand[4];
    u8 currWeapon;
    Weapon weapons[8];
    u16 lives;
    u32 money;
    u16 unk2;
    u16 unk3;
    u16 currentHp;
    u16 totalHp;
    u8 unk4;
    std::string onGameoverLevel;
    std::string currentLevel;
    u16 unk5;
    u8 unk6[0x19];
    u32 xPos;
    u32 yPos;
    u16 unk7;
    u32 facingDirection;
    u8 unk8[5];
    std::string checkpointStage;
    std::string checkpointTeleporterParameter;
    // TODO!!!
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
    ProfileSave saves[NUM_SAVES];

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