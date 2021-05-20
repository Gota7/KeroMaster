#include "raylib.h"
#include "inicpp.h"
#include "settings.h"
#include "../gbin/gfile.h"
#include "../rlImGui/fileBrowser.h"
#include "../rlImGui/utils.h"

using namespace std;
using namespace ini;
using namespace imgui_addons;

void Settings::Load()
{
    if (GFile::FileExists("settings.ini"))
    {
        IniFile f;
        f.load("settings.ini");
        rscPath = f["Settings"]["RscPath"].as<string>();
        lastLevel = f["Settings"]["LastLevel"].as<string>();
        leftClick = (EditorTool)f["Settings"]["LeftClick"].as<int>();
        rightClick = (EditorTool)f["Settings"]["RightClick"].as<int>();
        middleClick = (EditorTool)f["Settings"]["MiddleClick"].as<int>();
        usePalette = f["Settings"]["UsePalette"].as<bool>();
        style.Load(f["Settings"]["Style"].as<string>());
    }
}

void Settings::Save()
{
    IniFile f;
    f["Settings"]["RscPath"] = rscPath;
    f["Settings"]["LastLevel"] = lastLevel;
    f["Settings"]["LeftClick"] = (int)leftClick;
    f["Settings"]["RightClick"] = (int)rightClick;
    f["Settings"]["MiddleClick"] = (int)middleClick;
    f["Settings"]["UsePalette"] = usePalette;
    f["Settings"]["Style"] = style.name;
    f.save("settings.ini");
}

const char* ActionTypes[] = {
    "Hand",
    "Tile Brush",
    "Eraser",
    "Entity Hand"
};

void Settings::ShowWindow(Editor* ed)
{
    static ImGuiFileBrowser fB;
    static int numFiles;
    static char** files;
    static int actionMouse = 0;
    if (ImGui::BeginPopupModal("Editor Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | (show ? ImGuiWindowFlags_MenuBar : 0)))
    {
        ed->focus.ObserveFocus();
        ed->focus.isModal |= true;
        if (show)
        {
            ed->DrawMainMenu(true);
        }
        if (ImGuiStringEdit("Resource Path", &rscPath))
        {
            lastLevel = "";
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("...##Resource Path"))
        {
            ImGui::OpenPopup("Open Resource Path");
        }
        if (strcmp(rscPath.c_str(), "") != 0)
        {
            ImGui::Text("%s", ("Level To Open: " + lastLevel).c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton("...##Last Level"))
            {
                ImGui::OpenPopup("Level Select");
                files = GetDirectoryFiles((rscPath + "/field").c_str(), &numFiles);
                qsort(files, numFiles, sizeof(char*), cmpstr);
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            }
            if (ImGui::BeginPopupModal("Level Select", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ed->focus.ObserveFocus();
                ed->focus.isModal |= true;
                ImGui::BeginListBox("Levels", ImVec2(300, 500));
                for (int i = 2; i < numFiles; i++)
                {
                    bool dummy = false;
                    const char* name = GetFileNameWithoutExt(files[i]);
                    if (ImGui::Selectable(name, &dummy))
                    {
                        lastLevel = name;
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndListBox();
                ImGui::EndPopup();
            }
            if (ImGui::Button("Update Object Data Folder"))
            {
                ImGui::OpenPopup("Notice");
            }
            if (ImGui::BeginPopupModal("Notice", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Go to https://github.com/Gota7/KeroMaster and download the object_data folder.\nReplace it with the current one, then restart the editor for changes to take effect!");
                if (ImGui::Button("Ok"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            if (strcmp(lastLevel.c_str(), "") != 0 && GFile::FileExists((rscPath + "/field/" + lastLevel + ".pxpack").c_str()))
            {
                ImGui::Separator();
                ImGui::Text("Action Button:");
                if (ImGui::RadioButton("Left Click", &actionMouse, 0))
                {
                    if (rightClick == (EditorTool)4) { rightClick = (EditorTool)0; }
                    if (middleClick == (EditorTool)4) { middleClick = (EditorTool)0; }
                    leftClick = (EditorTool)4;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Right Click", &actionMouse, 1))
                {
                    if (leftClick == (EditorTool)4) { leftClick = (EditorTool)0; }
                    if (middleClick == (EditorTool)4) { middleClick = (EditorTool)0; }
                    rightClick = (EditorTool)4;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Middle Click", &actionMouse, 2))
                {
                    if (leftClick == (EditorTool)4) { leftClick = (EditorTool)0; }
                    if (rightClick == (EditorTool)4) { rightClick = (EditorTool)0; }
                    middleClick = (EditorTool)4;
                }
                if (actionMouse != 0)
                {
                    ImGui::Combo("Left Click Action", (int*)&leftClick, ActionTypes, IM_ARRAYSIZE(ActionTypes));
                }
                if (actionMouse != 1)
                {
                    ImGui::Combo("Right Click Action", (int*)&rightClick, ActionTypes, IM_ARRAYSIZE(ActionTypes));
                }
                if (actionMouse != 2)
                {
                    ImGui::Combo("Middle Click Action", (int*)&middleClick, ActionTypes, IM_ARRAYSIZE(ActionTypes));
                }
                ImGui::Separator();
                ImGui::Checkbox("Use Tile Palette", &usePalette);
                ImGui::Separator();
                if (ImGui::Button("Save And Close"))
                {
                    Save();
                    ed->SetPath(rscPath);
                    if (strcmp(ed->mapName.c_str(), lastLevel.c_str()) != 0)
                    {
                        ed->LoadLevel(lastLevel);
                    }
                    show = false;
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        if (fB.showFileDialog("Open Resource Path", ImGuiFileBrowser::DialogMode::SELECT))
        {
            rscPath = fB.selected_path;
            lastLevel = "";
        }
        ImGui::EndPopup();
    }
    
}

bool Settings::ButtonDown(EditorTool tool)
{
    if (leftClick == tool)
    {
        return IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    }
    else if (rightClick == tool)
    {
        return IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    }
    else if (middleClick == tool)
    {
        return IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);
    }
    else
    {
        return false;
    }
}

bool Settings::ButtonUp(EditorTool tool)
{
    if (leftClick == tool)
    {
        return IsMouseButtonUp(MOUSE_LEFT_BUTTON);
    }
    else if (rightClick == tool)
    {
        return IsMouseButtonUp(MOUSE_RIGHT_BUTTON);
    }
    else if (middleClick == tool)
    {
        return IsMouseButtonUp(MOUSE_MIDDLE_BUTTON);
    }
    else
    {
        return true;
    }
}

bool Settings::ButtonPressed(EditorTool tool)
{
    if (leftClick == tool)
    {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
    else if (rightClick == tool)
    {
        return IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
    }
    else if (middleClick == tool)
    {
        return IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON);
    }
    else
    {
        return false;
    }
}

bool Settings::ButtonReleased(EditorTool tool)
{
    if (leftClick == tool)
    {
        return IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    }
    else if (rightClick == tool)
    {
        return IsMouseButtonReleased(MOUSE_RIGHT_BUTTON);
    }
    else if (middleClick == tool)
    {
        return IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON);
    }
    else
    {
        return false;
    }
}

bool Settings::ButtonClicked(EditorTool tool, bool repeat)
{
    if (leftClick == tool)
    {
        return ImGui::IsMouseClicked(ImGuiMouseButton_Left, repeat);
    }
    else if (rightClick == tool)
    {
        return ImGui::IsMouseClicked(ImGuiMouseButton_Right, repeat);
    }
    else if (middleClick == tool)
    {
        return ImGui::IsMouseClicked(ImGuiMouseButton_Middle, repeat);
    }
    else
    {
        return false;
    }
}