#include "profile.h"

bool Profile::FileExists(std::string rsc_k, std::string name)
{
    return GFile::FileExists((rsc_k + "/profile/" + name).c_str());
}

GFile Profile::GetGFile(std::string rsc_k, std::string name)
{
    return GFile((rsc_k + "/profile/" + name).c_str());
}

void Profile::ReadAll(std::string rsc_k)
{
    LoadFullscreen(rsc_k);
    LoadGameProfile(rsc_k);
    LoadControlInterfaceMode(rsc_k);
    LoadRegion(rsc_k);
    LoadRect(rsc_k);
    LoadLastIdx(rsc_k);
    for (int i = 0; i < NUM_SAVES; i++)
    {
        LoadProfileSave(rsc_k, i);
    }
    LoadSize(rsc_k);
    LoadLastTime(rsc_k);
    LoadKeyConfig(rsc_k);
}

void Profile::LoadFullscreen(std::string rsc_k)
{
    if (!FileExists(rsc_k, "fullscreen.bin")) return;
    GFile f = GetGFile(rsc_k, "fullscreen.bin");
    fullscreen = f.ReadU8();
    f.Close();
}

void Profile::SaveFullscreen(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "fullscreen.bin");
    f.Clear();
    f.Write(fullscreen);
    f.Close();
}

void Profile::LoadGameProfile(std::string rsc_k)
{
    if (!FileExists(rsc_k, "gameprofile")) return;
    GFile f = GetGFile(rsc_k, "gameprofile");
    f.ReadU64();
    u32 max = f.ReadU32();
    for (int i = 0; i < max; i++)
    {
        gameProfile[i] = f.ReadU8();
    }
    f.Close();
}

void Profile::SaveGameProfile(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "gameprofile");
    f.Clear();
    f.WriteNullTerminated("140214a");
    f.Write((u32)NUM_GAME_FLAGS);
    for (int i = 0; i < NUM_GAME_FLAGS; i++)
    {
        f.Write(gameProfile[i]);
    }
    f.Close();
}

void Profile::LoadControlInterfaceMode(std::string rsc_k)
{
    if (!FileExists(rsc_k, "interfacemode.bin")) return;
    GFile f = GetGFile(rsc_k, "interfacemode.bin");
    useJoystick = f.ReadU32() > 0;
    f.Close();
}

void Profile::SaveControlInterfaceMode(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "interfacemode.bin");
    f.Clear();
    f.Write((u32)useJoystick);
    f.Close();
}

void Profile::LoadRegion(std::string rsc_k)
{
    if (!FileExists(rsc_k, "kero.region")) return;
    GFile f = GetGFile(rsc_k, "kero.region");
    u32 len = f.ReadU32();
    region = f.ReadStrFixed(len);
    f.Close();
}

void Profile::SaveRegion(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "kero.region");
    f.Clear();
    f.Write((u32)region.size());
    f.Write(region);
    f.Close();
}

void Profile::LoadRect(std::string rsc_k)
{
    if (!FileExists(rsc_k, "main.rect")) return;
    GFile f = GetGFile(rsc_k, "main.rect");
    topLeftCorner[0] = f.ReadS32();
    topLeftCorner[1] = f.ReadS32();
    bottomRightCorner[0] = f.ReadS32();
    bottomRightCorner[1] = f.ReadS32();
    f.Close();
}

void Profile::SaveRect(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "main.rect");
    f.Clear();
    f.Write(topLeftCorner[0]);
    f.Write(topLeftCorner[1]);
    f.Write(bottomRightCorner[0]);
    f.Write(bottomRightCorner[1]);
    f.Write((u32)1);
    f.Close();
}

void Profile::LoadLastIdx(std::string rsc_k)
{
    if (!FileExists(rsc_k, "profidx")) return;
    GFile f = GetGFile(rsc_k, "profidx");
    lastProfileSave = f.ReadU32();
    f.Close();
}

void Profile::SaveLastIdx(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "profidx");
    f.Clear();
    f.Write((u32)lastProfileSave);
    f.Close();
}

// TODO: GAME SAVE CODE!!!

void Profile::LoadProfileSave(std::string rsc_k, u8 num)
{

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

}

void Profile::SaveProfileSave(std::string rsc_k, u8 num)
{

}

void Profile::LoadSize(std::string rsc_k)
{
    if (!FileExists(rsc_k, "screensize.bin")) return;
    GFile f = GetGFile(rsc_k, "screensize.bin");
    screenSize = f.ReadU32();
    f.Close();
}

void Profile::SaveSize(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "screensize.bin");
    f.Clear();
    f.Write((u32)screenSize);
    f.Close();
}

void Profile::LoadLastTime(std::string rsc_k)
{
    if (!FileExists(rsc_k, "timelast")) return;
    GFile f = GetGFile(rsc_k, "timelast");
    lastYear = f.ReadU16();
    lastMonth = f.ReadU8();
    lastDay = f.ReadU8();
    lastHour = f.ReadU8();
    lastMinute = f.ReadU8();
    lastSecond = f.ReadU8();
    f.Close();
}

void Profile::SaveLastTime(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "timelast");
    f.Clear();
    f.Write(lastYear);
    f.Write(lastMonth);
    f.Write(lastDay);
    f.Write(lastHour);
    f.Write(lastMinute);
    f.Write(lastSecond);
    f.Write((u8)0);
    f.Close();
}

void Profile::LoadKeyConfig(std::string rsc_k)
{
    if (!FileExists(rsc_k, "user.keyconfig")) return;
    GFile f = GetGFile(rsc_k, "user.keyconfig");
    f.ReadU64();
    keyboardLeft = f.ReadU8();
    keyboardUp = f.ReadU8();
    keyboardRight = f.ReadU8();
    keyboardDown = f.ReadU8();
    keyboardJump = f.ReadU8();
    keyboardShoot = f.ReadU8();
    keyboardWeaponSwitchLeft = f.ReadU8();
    keyboardWeaponSwitchRight = f.ReadU8();
    keyboardMenu = f.ReadU8();
    joystickJump = f.ReadU8();
    joystickShoot = f.ReadU8();
    joystickWeaponSwitchLeft = f.ReadU8();
    joystickWeaponSwitchRight = f.ReadU8();
    joystickMenu = f.ReadU8();
    f.Close();
}

void Profile::SaveKeyConfig(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "user.keyconfig");
    f.Clear();
    f.WriteNullTerminated("130704k");
    f.Write(keyboardLeft);
    f.Write(keyboardUp);
    f.Write(keyboardRight);
    f.Write(keyboardDown);
    f.Write(keyboardJump);
    f.Write(keyboardShoot);
    f.Write(keyboardWeaponSwitchLeft);
    f.Write(keyboardWeaponSwitchRight);
    f.Write(keyboardMenu);
    f.Write(joystickJump);
    f.Write(joystickShoot);
    f.Write(joystickWeaponSwitchLeft);
    f.Write(joystickWeaponSwitchRight);
    f.Write(joystickMenu);
    f.Close();
}