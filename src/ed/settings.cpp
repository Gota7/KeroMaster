#include "settings.h"
#include "editor.h"
#include "inicpp.h"
#include "../gbin/gfile.h"
#include "../rlImGui/fileBrowser.h"
#include "../rlImGui/utils.h"
#include "imgui.h"
#include <string>
#include <vector>

void Settings::Load()
{
    if (GFile::FileExists("settings.ini"))
    {
        ini::IniFile f;
        f.load("settings.ini");
        u16 ldMajor = f["Version"]["Major"].as<u16>();
        u16 ldMinor = f["Version"]["Minor"].as<u16>();
        u16 ldRevision = f["Version"]["Revision"].as<u16>();

        // Always present settings.
        width = f["Settings"]["Width"].as<int>();
        height = f["Settings"]["Height"].as<int>();
        maximized = f["Settings"]["Maximized"].as<bool>();
        rscPath = f["Settings"]["RscPath"].as<std::string>();
        lastLevel = f["Settings"]["LastLevel"].as<std::string>();
        usePalette = f["Settings"]["UsePalette"].as<bool>();
        leftClick = (ToolItem)f["Settings"]["LeftClick"].as<int>();
        rightClick = (ToolItem)f["Settings"]["RightClick"].as<int>();
        middleClick = (ToolItem)f["Settings"]["MiddleClick"].as<int>();
        currStyle = f["Settings"]["Style"].as<std::string>();
        showPlayArea = f["Settings"]["ShowPlayArea"].as<bool>();
        showGrid = f["Settings"]["ShowGrid"].as<bool>();
        viewLayers[0] = f["Settings"]["ViewLayer0"].as<bool>();
        viewLayers[1] = f["Settings"]["ViewLayer1"].as<bool>();
        viewLayers[2] = f["Settings"]["ViewLayer2"].as<bool>();
        viewEntityImages = f["Settings"]["ViewEntityImages"].as<bool>();
        viewEntityBoxes = f["Settings"]["ViewEntityBoxes"].as<bool>();
        viewEntities = f["Settings"]["ViewEntities"].as<bool>();
        //viewTileAttributes = f["Settings"]["ViewTileAttributes"].as<bool>();
        openTilesetsOnLoad = f["Settings"]["OpenTilesetsOnLoad"].as<bool>();
        maxUndoDepth = f["Settings"]["MaxUndoDepth"].as<int>();
    }
    else
    {
        currStyle = "Cat & Frog";
    }

    // Test to see if the settings window needs to be shown.
    if (rscPath == "" || lastLevel == "") show = true;
    else if (!GFile::FileExists((rscPath + "/field/" + lastLevel + ".pxpack").c_str())) show = true;


}

void Settings::Save()
{
    ini::IniFile f;
    f["Version"]["Major"] = versionMajor;
    f["Version"]["Minor"] = versionMinor;
    f["Version"]["Revision"] = versionRevision;
    f["Settings"]["Width"] = width;
    f["Settings"]["Height"] = height;
    f["Settings"]["Maximized"] = maximized;
    f["Settings"]["RscPath"] = rscPath;
    f["Settings"]["LastLevel"] = lastLevel;
    f["Settings"]["UsePalette"] = usePalette;
    f["Settings"]["LeftClick"] = (int)leftClick;
    f["Settings"]["RightClick"] = (int)rightClick;
    f["Settings"]["MiddleClick"] = (int)middleClick;
    f["Settings"]["Style"] = currStyle;
    f["Settings"]["ShowPlayArea"] = showPlayArea;
    f["Settings"]["ShowGrid"] = showGrid;
    f["Settings"]["ViewLayer0"] = viewLayers[0];
    f["Settings"]["ViewLayer1"] = viewLayers[1];
    f["Settings"]["ViewLayer2"] = viewLayers[2];
    f["Settings"]["ViewEntityImages"] = viewEntityImages;
    f["Settings"]["ViewEntityBoxes"] = viewEntityBoxes;
    f["Settings"]["ViewEntities"] = viewEntities;
    //f["Settings"]["ViewTileAttributes"] = viewTileAttributes;
    f["Settings"]["OpenTilesetsOnLoad"] = openTilesetsOnLoad;
    f["Settings"]["MaxUndoDepth"] = maxUndoDepth;
    f.save("settings.ini");
}

void Settings::DrawUI(Editor* ed)
{

    // Open check.
    if (!show && !softShow) return;

    // Settings data.
    static imgui_addons::ImGuiFileBrowser fB;
    static int numFiles;
    static char** files = nullptr;
    static int actionMouse = 0;

    // Show the settings popup.
    if (ImGui::BeginPopupModal("Editor Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | (show ? 0/*ImGuiWindowFlags_MenuBar*/ : 0)))
    {

        // Resource path choosing.
        ed->focus.ObserveFocus();
        ed->focus.isModal |= true;
        if (show)
        {
            //ed->DrawMainMenu(true);
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

        // Resource path available.
        if (rscPath != "")
        {
            ImGui::Text("%s", ("Level To Open: " + lastLevel).c_str());
            ImGui::SameLine();

            // Get the level list.
            if (ImGui::SmallButton("...##Last Level"))
            {
                ImGui::OpenPopup("Level Select");
                std::vector<std::string> levelFiles = ReadFilesFromDir(rscPath + "/field");
                std::sort(levelFiles.begin(), levelFiles.end());
                if (files != nullptr) DelImGuiStringList(files, numFiles);
                files = GenImGuiStringList(levelFiles, &numFiles);
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            }

            // Level selection box.
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

            // Updating.
            if (ImGui::Button("Update Object Data Folder"))
            {
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/all.xml -o object_data/all.xml");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/alwaysLoaded.txt -o object_data/alwaysLoaded.txt");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/attribute.png -o object_data/attribute.png");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/default.pxeve -o object_data/default.pxeve");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/scriptInfo.xml -o object_data/scriptInfo.xml");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/unittype.png -o object_data/unittype.png");
                system("curl https://raw.githubusercontent.com/Gota7/KeroMaster/main/object_data/unittype.txt -o object_data/unittype.txt");
                ImGui::OpenPopup("Notice");
            }
            ImGuiTooltip("System must have curl command available for this to work!");
            if (ImGui::BeginPopupModal("Notice", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Restart the editor for changes to take effect!");
                if (ImGui::Button("Ok"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // Since level is available, get the actions set up.
            if (lastLevel != "" && GFile::FileExists((rscPath + "/field/" + lastLevel + ".pxpack").c_str()))
            {
                ImGui::Separator();
                ImGui::Text("Action Button:");
                if (ImGui::RadioButton("Left Click", &actionMouse, 0))
                {
                    if (rightClick == ToolItem::CurrentTool) { rightClick = ToolItem::Hand; }
                    if (middleClick == ToolItem::CurrentTool) { middleClick = ToolItem::Hand; }
                    leftClick = ToolItem::CurrentTool;
                }
                ImGuiTooltip("The mouse button that will do the current tool's action.");
                ImGui::SameLine();
                if (ImGui::RadioButton("Right Click", &actionMouse, 1))
                {
                    if (leftClick == ToolItem::CurrentTool) { leftClick = ToolItem::Hand; }
                    if (middleClick == ToolItem::CurrentTool) { middleClick = ToolItem::Hand; }
                    rightClick = ToolItem::CurrentTool;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Middle Click", &actionMouse, 2))
                {
                    if (leftClick == ToolItem::CurrentTool) { leftClick = ToolItem::Hand; }
                    if (rightClick == ToolItem::CurrentTool) { rightClick = ToolItem::Hand; }
                    middleClick = ToolItem::CurrentTool;
                }
                if (actionMouse != 0)
                {
                    ImGui::Combo("Left Click Action", (int*)&leftClick, Tools::toolNames, (int)ToolItem::NumTools);
                }
                if (actionMouse != 1)
                {
                    ImGui::Combo("Right Click Action", (int*)&rightClick, Tools::toolNames, (int)ToolItem::NumTools);
                }
                if (actionMouse != 2)
                {
                    ImGui::Combo("Middle Click Action", (int*)&middleClick, Tools::toolNames, (int)ToolItem::NumTools);
                }
                ImGui::Separator();
                ImGui::Checkbox("Use Tile Palette", &usePalette);
                ImGuiTooltip("Use the current layer's palette to draw in addition to from tilesets.");
                ImGui::Checkbox("Open Tilesets On Load", &openTilesetsOnLoad);
                ImGuiTooltip("When loading a level, open all of its tilesets.");
                ImGui::InputScalar("Undo Stack Size", ImGuiDataType_S32, &maxUndoDepth);
                ImGuiTooltip("How many individual allow actions to allow (bigger stack is more RAM usage).");
                ImGui::Separator();
                if (ImGui::Button("Save And Close"))
                {
                    Save();
                    ed->InitEditor();
                    show = false;
                    softShow = false;
                    ImGui::CloseCurrentPopup();
                }
                if (softShow && (ImGui::SameLine(), true) && ImGui::Button("Cancel"))
                {
                    Load();
                    show = false;
                    softShow = false;
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        if (fB.showFileDialog("Open Resource Path", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT))
        {
            rscPath = fB.selected_path;
            lastLevel = "";
        }
        ImGui::EndPopup();
    }

}