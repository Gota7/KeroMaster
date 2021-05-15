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
    u32 useJoystick = 0;
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
    u8 profileDataBits = 0;
    bool zangyouUnlocked = false;
    u8 filesStarted = 0;
    u8 achievements0 = 0;
    u8 achievements1 = 0;
    u8 viewedAchievements0 = 0;
    u8 viewedAchievements1 = 0;
    //u8 gameProfileUnknown[]
    std::vector<ProfileSave> saves = std::vector<ProfileSave>();

    GFile GetGFile(std::string rsc_k, std::string name);
    void ReadAll(std::string rsc_k);
    void SaveFullscreen(std::string rsc_k);
    void SaveRegion(std::string rsc_k);
    void SaveRect(std::string rsc_k);
    void SaveLastIdx(std::string rsc_k);
    void SaveSize(std::string rsc_k);
    void SaveLastTime(std::string rsc_k);
    void SaveKeyConfig(std::string rsc_k);
    void SaveControlInterfaceMode(std::string rsc_k);
    void SaveGameProfile(std::string rsc_k);
    void SaveProfileSave(std::string rsc_k, u8 num);
};