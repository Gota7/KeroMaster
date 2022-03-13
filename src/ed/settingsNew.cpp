#include "settingsNew.h"
#include "editorNew.h"
#include "inicpp.h"
#include "../gbin/gfile.h"

void SettingsNew::Load()
{
    if (GFile::FileExists("settings.ini"))
    {
        ini::IniFile f;
        f.load("settings.ini");
        width = f["Settings"]["Width"].as<int>();
        height = f["Settings"]["Height"].as<int>();
        maximized = f["Settings"]["Maximized"].as<bool>();
        rscPath = f["Settings"]["RscPath"].as<std::string>();
        lastLevel = f["Settings"]["LastLevel"].as<std::string>();
        leftClick = (ToolItem)f["Settings"]["LeftClick"].as<int>();
        rightClick = (ToolItem)f["Settings"]["RightClick"].as<int>();
        middleClick = (ToolItem)f["Settings"]["MiddleClick"].as<int>();
        usePalette = f["Settings"]["UsePalette"].as<bool>();
        currStyle = f["Settings"]["Style"].as<std::string>();
    }
    else
    {
        currStyle = "Cat & Frog";
    }
}

void SettingsNew::Save()
{
    ini::IniFile f;
    f["Settings"]["Width"] = width;
    f["Settings"]["Height"] = height;
    f["Settings"]["Maximized"] = maximized;
    f["Settings"]["RscPath"] = rscPath;
    f["Settings"]["LastLevel"] = lastLevel;
    f["Settings"]["LeftClick"] = (int)leftClick;
    f["Settings"]["RightClick"] = (int)rightClick;
    f["Settings"]["MiddleClick"] = (int)middleClick;
    f["Settings"]["UsePalette"] = usePalette;
    f["Settings"]["Style"] = currStyle;
    f.save("settings.ini");
}

void SettingsNew::DrawUI(EditorNew* ed)
{

    // Open check.
    if (!show) return;

    

}