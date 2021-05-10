#pragma once

#include <string>
#include <vector>
#include <imgui.h>
#include "../types.h"

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
    u8 month = 5;
    u8 lastDay = 8;
    u8 lastHour = 21;
    u8 lastSecond = 16;
    u8 lastProfileSave = 5;
    u32 useJoystick = 0;
    std::vector<ProfileSave> saves = std::vector<ProfileSave>();

    void ReadAll(std::string rsc_k);
    void SaveFullscreen(std::string rsc_k);
    void SaveRegion(std::string rsc_k);
    void SaveRect(std::string rsc_k);
    void SaveLastIdx(std::string rsc_k);
    void SaveSize(std::string rsc_k);
    void SaveLastTime(std::string rsc_k);
    void SaveKeyConfig(std::string rsc_k);
    void SaveControlInterfaceMode(std::string rsc_k);
    void SaveProfileSave(std::string rsc_k, u8 num);
};