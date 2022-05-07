#include "levelEditor.h"
#include "editor.h"
#include "../rlImGui/utils.h"
#include "imgui.h"

const char* scrollTypes[] = {
    "Normal",
    "3/4",
    "Half",
    "Quarter",
    "Eighth",
    "Zero",
    "H 3/4",
    "H Half",
    "H Quater",
    "V0 Half"
};

const char* tileScales[] = {
    "0 (Undefined - Falls back to 2)",
    "1 (16x16)",
    "2 (8x8)",
    "3 (5.33x5.33)",
    "4 (4x4)",
    "5 (3.2x3.2)",
    "6 (2.66x2.66)",
    "7 (2.29x2.29)",
    "8 (2x2)",
    "9 (1.77x1.77)",
    "10 (1.6x1.6)",
    "11 (1.45x1.45)",
    "12 (1.33x1.33)",
    "13 (1.23x1.23)",
    "14 (1.14x1.14)",
    "15 (1.06x1.06)",
    "16 (1x1)"
};

LevelEditor::LevelEditor(Editor* ed)
{
    this->ed = ed;
}

void LevelEditor::DrawUI()
{

    // Open check.
    if (!open) return;

    // Editor.
    auto& io = ImGui::GetIO();

    // Initialize editor.
    ImGui::Begin(("Level Editor - " + ed->level + "###EditorDialog").c_str(), &open);
    ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - 330.0f, 30.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();

    // General parameters.
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Title", &ed->map.levelTitle.dat);
    ImGuiTooltip("Internal name for the level.\nThis does not seem important?");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Left Level", &ed->map.references[RT_LEFT_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears to the left of this level.");
    EditLevelButton(RT_LEFT_LEVEL);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Right Level", &ed->map.references[RT_RIGHT_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears to the right of this level.");
    EditLevelButton(RT_RIGHT_LEVEL);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Upper Level", &ed->map.references[RT_UP_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears above this level.");
    EditLevelButton(RT_UP_LEVEL);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Lower Level", &ed->map.references[RT_DOWN_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears below this level.");
    EditLevelButton(RT_DOWN_LEVEL);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("NPC Palette", &ed->map.references[RT_NPC_PALETTE].dat);
    ImGuiTooltip("Name of the .png in the img folder for NPCs to use (that depend on the NPC palette).");
    ImGui::Separator();
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area X", ImGuiDataType_U16, &ed->map.levelSettings[0]);
    ImGuiTooltip("Start X tile for the area (usually 0).\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Y", ImGuiDataType_U16, &ed->map.levelSettings[2]);
    ImGuiTooltip("Start Y tile for the area (usually 0).\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Number", ImGuiDataType_U8, &ed->map.levelSettings[4]);
    ImGuiTooltip("Which area the level is a part of.\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);
    ImGuiColorEdit("Background Color", &ed->map.levelSettings[5]);
    ImGuiTooltip("Color to display in the background.");

    // Tileset settings.
    for (int i = 0 ; i < NUM_TILESETS; i++)
    {
        ImGui::Separator();
        DrawTilesetSettings(i, itemWidth);
    }

    // Tileset resizing.
    for (int i = 0; i < NUM_TILESETS; i++)
    {
        ImGui::Separator();
        ImGui::PushItemWidth(itemWidth / 2 - 5);
        ImGui::InputScalar(("##MapWidth" + std::to_string(i)).c_str(), ImGuiDataType_U16, &ed->map.maps[i].newWidth);
        ImGui::SameLine(0, -5.0);
        ImGui::PushItemWidth(itemWidth / 2 - 5);
        ImGui::InputScalar(("Map " + std::to_string(i) + " Size##MapHeight" + std::to_string(i)).c_str(), ImGuiDataType_U16, &ed->map.maps[i].newHeight);
        ImGui::SameLine();
        if (ImGui::SmallButton(("Resize##Map" + std::to_string(i)).c_str()))
        {
            resizeWarning = false;
            for (int x = ed->map.maps[i].newWidth; x < ed->map.maps[i].width; x++)
            {
                for (int y = 0; y < ed->map.maps[i].height; y++)
                {
                    if (ed->map.maps[i].GetTile(x, y) != 0)
                    {
                        resizeMapLayer = i;
                        resizeMode = ShiftDirection::Resize;
                        resizeWarning = true;
                        break;
                    }
                }
            }
            for (int y = ed->map.maps[i].newHeight; y < ed->map.maps[i].height; y++)
            {
                for (int x = 0; x < ed->map.maps[i].width; x++)
                {
                    if (ed->map.maps[i].GetTile(x, y) != 0)
                    {
                        resizeMapLayer = i;
                        resizeMode = ShiftDirection::Resize;
                        resizeWarning = true;
                        break;
                    }
                }
            }
            if (!resizeWarning)
            {
                ed->map.maps[i].Resize(ed->map.maps[i].newWidth, ed->map.maps[i].newHeight);
            }
        }
        if (ImGui::SmallButton(("Move Up##Map" + std::to_string(i)).c_str()))
        {
            CanShiftInDirCheck(ShiftDirection::Up, i);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Down##Map" + std::to_string(i)).c_str()))
        {
            CanShiftInDirCheck(ShiftDirection::Down, i);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Left##Map" + std::to_string(i)).c_str()))
        {
            CanShiftInDirCheck(ShiftDirection::Left, i);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Right##Map" + std::to_string(i)).c_str()))
        {
            CanShiftInDirCheck(ShiftDirection::Right, i);
        }
    }

    // Script editing.
    int numButtons = 0;
    ImGui::Separator();
    if (ed->level != "")
    {
        if (ImGui::Button("Edit Script"))
        {
            ed->OpenScript(ed->level);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        }
        else
        {
            numButtons = 0;
        }
    }

    // Reload tilesets.
    if (ImGui::Button("Reload Tilesets"))
    {
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            ed->tilesets[ed->map.tilesets[i].dat].Unload();
            ed->tilesets.erase(ed->map.tilesets[i].dat);
        }
        ed->tilesets[ed->map.references[RT_NPC_PALETTE].dat].Unload();
        ed->tilesets.erase(ed->map.references[RT_NPC_PALETTE].dat);
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            Tileset t;
            t.Load(ed->rsc, ed->map.tilesets[i].dat);
            ed->tilesets[ed->map.tilesets[i].dat] = t;
        }
        Tileset t2;
        t2.Load(ed->rsc, ed->map.references[RT_NPC_PALETTE].dat);
        ed->tilesets[ed->map.references[RT_NPC_PALETTE].dat] = t2;
    }
    ImGui::End();

    // Draw warning.
    DrawMapShiftResizeWarning(itemWidth);

}

void LevelEditor::DrawTilesetSettings(int tilesetNum, int itemWidth)
{

    // Edit tileset tile size.
    ImGui::PushItemWidth(itemWidth);
    int index = ed->map.tilesetSettings1[tilesetNum];
    if (ImGui::Combo(("Tileset " + std::to_string(tilesetNum) + " Tile Size").c_str(), &index, tileScales, IM_ARRAYSIZE(tileScales)))
    {
        ed->map.tilesetSettings1[tilesetNum] = (u8)index;
    }
    ImGuiTooltip("Size of how the tiles should be used.");

    // Tileset scroll type.
    int tmpScrollType = ed->map.tilesetSettings2[tilesetNum];
    if (ImGui::Combo(("Tileset " + std::to_string(tilesetNum) + " Scroll Mode").c_str(), &tmpScrollType, scrollTypes, IM_ARRAYSIZE(scrollTypes)))
    {
        ed->map.tilesetSettings2[tilesetNum] = (u8)tmpScrollType;
    }
    ImGuiTooltip("How to scroll tiles when the camera moves.");
    ImGui::PushItemWidth(itemWidth);

    // Tileset name editor.
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit(("Tileset " + std::to_string(tilesetNum)).c_str(), &ed->map.tilesets[tilesetNum].dat);
    if (ed->map.tilesets[tilesetNum].dat != "")
    {
        ImGui::SameLine();
        if (ImGui::SmallButton(("Open##Tileset" + std::to_string(tilesetNum)).c_str()))
        {
            ed->OpenTileset(ed->map.tilesets[tilesetNum].dat, 8.0f); // TODO: MAKE THIS HAVE THE CORRECT SIZE!!!
        }
    }
    ImGuiTooltip("Name of the tileset to use in the img folder.");

}

void LevelEditor::CanShiftInDirCheck(ShiftDirection dir, int layer)
{
    resizeWarning = !ed->map.maps[layer].CanShift(dir);
    if (resizeWarning)
    {
        resizeMode = dir;
        resizeMapLayer = layer;
    }
    else
    {
        ed->map.maps[layer].Shift(dir);
    }
}

void LevelEditor::EditLevelButton(int id)
{
    if (ed->map.references[id].dat != "")
    {
        ImGui::SameLine();
        if (ImGui::Button(("Edit##Lvl" + std::to_string(id)).c_str()))
        {
            ed->level = ed->map.references[id].dat;
            ed->LoadLevel();
        }
    }
}

void LevelEditor::DrawMapShiftResizeWarning(int itemWidth)
{

    // Warning for resizing.
    if (resizeWarning)
    {
        resizeWarning = false;
        ImGui::OpenPopup("Map Warning");
    }
    if (ImGui::BeginPopupModal("Map Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ed->focus.ObserveFocus();
        ed->focus.isModal |= true;
        ImGui::Text("This map operation will lose data.\nAre you sure you want to continue?");
        if (ImGui::Button("Yes", ImVec2(itemWidth / 1.33, 0)))
        {
            switch (resizeMode)
            {
                case ShiftDirection::Up:
                case ShiftDirection::Down:
                case ShiftDirection::Left:
                case ShiftDirection::Right:
                    ed->map.maps[resizeMapLayer].Shift(resizeMode);
                    break;
                case ShiftDirection::Resize:
                    ed->map.maps[resizeMapLayer].Resize(ed->map.maps[resizeMapLayer].newWidth, ed->map.maps[resizeMapLayer].newHeight);
                    break;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - itemWidth / 1.33 - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::Button("No", ImVec2(itemWidth / 1.33, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

}