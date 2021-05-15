#include "profile.h"

using namespace std;

GFile Profile::GetGFile(std::string rsc_k, string name)
{
    return GFile((rsc_k + "/profile/" + name).c_str());
}

void Profile::ReadAll(string rsc_k)
{

}

void Profile::SaveFullscreen(string rsc_k)
{
    GFile f = GetGFile(rsc_k, "fullscreen.bin");
    f.Clear();
    f.Write(fullscreen);
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

void Profile::SaveLastIdx(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "profidx");
    f.Clear();
    f.Write((u32)lastProfileSave);
    f.Close();
}

void Profile::SaveSize(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "profidx");
    f.Clear();
    f.Write((u32)screenSize);
    f.Close();
}

void Profile::SaveLastTime(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "timelast");
    f.Clear();
    f.Write((u16)lastYear);
    f.Write(lastMonth);
    f.Write(lastDay);
    f.Write(lastHour);
    f.Write(lastMinute);
    f.Write(lastSecond);
    f.Write((u8)0);
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

void Profile::SaveControlInterfaceMode(std::string rsc_k)
{
    GFile f = GetGFile(rsc_k, "interfacemode.bin");
    f.Clear();
    f.Write((u32)useJoystick);
    f.Close();
}