#include <iostream>
#include <fstream>
#include <imgui.h>
#include "editor.h"
#include "../rlImGui/fileBrowser.h"
#include "../rlImGui/utils.h"

std::map<string, Tileset> Editor::tilesets;
std::map<u8, EntityDisplay> Editor::entities;

using namespace imgui_addons;

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa, bb);
}

void Editor::SetPath(string rsc)
{
    this->rsc = rsc;
}

void Editor::LoadEnemies(string xmlName)
{
    entities = LoadXML(xmlName);
    entityListing = new char[5000];
    int pos = 0;
    for (u16 i = 0; i <= 255; i++)
    {
        string toCopy = entities.find(i) != entities.end() ? (to_string(i) + " - " + entities[i].name) : (to_string(i) + " - Unknown");
        strcpy(entityListing + pos, toCopy.c_str());
        pos += strlen(toCopy.c_str()) + 1;
    }
}

void Editor::LoadTileset(string tilesetName)
{
    Tileset t;
    t.Load(rsc, tilesetName);
    tilesets[tilesetName] = t;
}

void Editor::LoadFixedTilesets()
{
    fstream f;
    f.open("object_data/alwaysLoaded.txt", ios::in);
    string s;
    while (getline(f, s))
    {
        LoadTileset(s);
    }
    f.close();
    Tileset::attrTex = LoadTexture("object_data/attribute.png");
}

void Editor::LoadLevel(string name)
{
    if (enabled)
    {
        map.Unload(tilesets);
    }
    mapName = name;
    cam.offset = { 0, 0 };
    cam.rotation = 0;
    cam.target = { 0, 0 };
    cam.zoom = 1;
    origin = { 0, 0 };
    map.Load(rsc, name, tilesets);
    enabled = true;
    for (int i = 0; i < NUM_TILESETS; i++)
    {
        if (strcmp(map.tilesets[i].dat.c_str(), "") != 0)
        {
            if (openTilesetsOnLoad) { tilesetEditors.push_back(TilesetEditor(this, map.tilesets[i].dat)); }
        }
    }
}

void Editor::UnloadLevel()
{
    map.Unload(tilesets);
}

void Editor::MoveCamX(float amount, bool relative)
{
    if (relative)
    {
        cam.offset.x += amount;
    }
    else
    {
        cam.offset.x = amount;
    }
}

void Editor::MoveCamY(float amount, bool relative)
{
    if (relative)
    {
        cam.offset.y += amount;
    }
    else
    {
        cam.offset.y = amount;
    }
}

void Editor::Draw()
{

    // Secondaries.
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        tilesetEditors[i].Draw();
    }

    // Safety.
    if (!enabled)
    {
        return;
    }

    // BG.
    map.Clear();

    // Push.
    BeginMode2D(cam);

    // Draw map.
    if (viewLayers[2]) map.DrawLayer(2, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewLayers[1]) map.DrawLayer(1, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewLayers[0]) map.DrawLayer(0, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewEntities) map.DrawEntities(entities, tilesets, origin, MAP_SIZE, viewEntityBoxes);

    // Play area.
    if (showPlayArea)
    {
        DrawRectangleLinesEx({ 0, 0, map.maps[0].width * MAP_SIZE * 8, map.maps[0].height * MAP_SIZE * 8 }, 3, RED);
    }

    // Grid.
    if (showGrid)
    {
        const float GRID_ALPHA = 0.1;
        for (int i = 0; i < map.maps[0].width; i++)
        {
            for (int j = 0; j < map.maps[0].height; j++)
            {
                Color c;
                if (i % 2 != j % 2)
                {
                    c = ColorFromNormalized({ 0.2, 0.2, 0.2, GRID_ALPHA });
                }
                else
                {
                    c = ColorFromNormalized({ 0.5, 0.5, 0.5, GRID_ALPHA });
                }
                DrawRectangle(i * 8 * MAP_SIZE, j * 8 * MAP_SIZE, 8 * MAP_SIZE, 8 * MAP_SIZE, c);
            }
        }
    }

    // Pop.
    EndMode2D();

}

void Editor::DrawUI()
{

    // Main menu.
    DrawMainMenu();

    // Secondaries.
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        tilesetEditors[i].DrawUI();
    }

    // Safety.
    if (!enabled)
    {
        return;
    }

    // Level editor.
    DrawLevelEditor();

    // Entity editor.
    DrawEntityEditor();

}

void Editor::DrawMainMenu()
{

    // Vars.
    static int numFiles;
    static char** files;
    bool openPopup = false;

    // File menu.
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                numFiles = 0;
                files = GetDirectoryFiles((rsc + "/field").c_str(), &numFiles);
                qsort(files, numFiles, sizeof(char*), cmpstr);
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                openPopup = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
            {
            }
            if (ImGui::MenuItem("Close", "Ctrl+Shift+C"))
            {
            }
            if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Play Area", &showPlayArea);
            ImGui::Checkbox("Grid", &showGrid);
            ImGui::Separator();
            ImGui::Checkbox("Layer 2", &viewLayers[2]);
            ImGui::Checkbox("Layer 1", &viewLayers[1]);
            ImGui::Checkbox("Layer 0", &viewLayers[0]);
            ImGui::Separator();
            ImGui::Checkbox("Entity Boxes", &viewEntityBoxes);
            ImGui::Checkbox("Entities", &viewEntities);
            ImGui::Separator();
            ImGui::Checkbox("Tile Attributes", &viewTileAttributes);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("About"))
        {   
        }
        ImGui::EndMainMenuBar();
    }

    // Level select.
    if (openPopup)
    {
        ImGui::OpenPopup("Select Level");
        openPopup = false;
    }
    if (ImGui::BeginPopupModal("Select Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGui::BeginListBox("Levels", ImVec2(300, 500));
        for (int i = 2; i < numFiles; i++)
        {
            bool dummy = false;
            const char* name = GetFileNameWithoutExt(files[i]);
            if (ImGui::Selectable(name, &dummy))
            {
                //UnloadLevel();
                LoadLevel(name);
            }
        }
        ImGui::EndListBox();
        ImGui::EndPopup();
    }

}

void Editor::DrawLevelEditor()
{

    // Editor.
    ImGui::Begin(("Level Editor - " + mapName).c_str());
    focus.ObserveFocus();
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Comment", &map.comment.dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Script", &map.references[0].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Next Level", &map.references[1].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Previous Level", &map.references[2].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Link Level", &map.references[3].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("NPC Palette", &map.references[4].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Unknown 0", ImGuiDataType_U16, &map.levelSettings[0]);
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Unknown 1", ImGuiDataType_U16, &map.levelSettings[2]);
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Scroll Mode", ImGuiDataType_U8, &map.levelSettings[4]);
    ImGui::PushItemWidth(itemWidth);
    ImGuiColorEdit("Background Color", &map.levelSettings[5]);
    for (int i = 0 ; i < NUM_TILESETS; i++)
    {
        ImGui::PushItemWidth(itemWidth);
        ImGui::InputScalar(("Tileset " + to_string(i) + " Settings 0").c_str(), ImGuiDataType_U8, &map.tilesetSettings1[i]);
        ImGui::InputScalar(("Tileset " + to_string(i) + " Settings 1").c_str(), ImGuiDataType_U8, &map.tilesetSettings2[i]);
        ImGui::PushItemWidth(itemWidth);
        ImGuiStringEdit(("Tileset " + to_string(i)).c_str(), &map.tilesets[i].dat);
        if (strcmp(map.tilesets[i].dat.c_str(), "") != 0)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Open"))
            {
                tilesetEditors.push_back(TilesetEditor(this, map.tilesets[i].dat));
            }
        }
    }
    int numButtons = 0;
    if (strcmp(map.references[0].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Script"))
        {

        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }
    if (strcmp(map.references[1].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Next Level"))
        {
            LoadLevel(map.references[1].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }
    if (strcmp(map.references[2].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Previous Level"))
        {
            LoadLevel(map.references[2].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }
    if (strcmp(map.references[3].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Link Level"))
        {
            LoadLevel(map.references[3].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }
    if (ImGui::Button("Reload Tilesets"))
    {

    }
    ImGui::End();

}

void Editor::DrawEntityEditor()
{
    if (editingEntity != NULL)
    {
        ImGui::Begin("Entity Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        focus.ObserveFocus();
        int currId = editingEntity->id;
        const int itemWidth = 150;
        ImGui::PushItemWidth(itemWidth);
        if (ImGui::Combo("Entity Id", &currId, entityListing))
        {
            editingEntity->id = currId;
        }
        ImGui::PushItemWidth(itemWidth);
        ImGui::InputScalar("Flags", ImGuiDataType_U8, &editingEntity->flags);
        ImGui::PushItemWidth(itemWidth);
        ImGui::InputScalar("Unknown", ImGuiDataType_U8, &editingEntity->unk);
        for (int i = 0; i < NUM_BYTE_PARAMETERS; i++)
        {
            ImGui::PushItemWidth(itemWidth);
            ImGui::InputScalar(("Parameter " + to_string(i)).c_str(), ImGuiDataType_U8, &editingEntity->parametersByte[i]);
        }
        for (int i = 0; i < NUM_PARAMETERS - NUM_BYTE_PARAMETERS; i++)
        {
            ImGui::PushItemWidth(itemWidth);
            ImGuiStringEdit(("Parameter " + to_string(i + NUM_BYTE_PARAMETERS)).c_str(), &editingEntity->parametersStr[i].dat);
        }
        ImGui::End();
    }
}

void Editor::Update()
{

    // Safety.
    if (!enabled)
    {
        return;
    }

    // Secondaries.
    for (int i = tilesetEditors.size() - 1; i >= 0; i--)
    {
        if (!tilesetEditors[i].open)
        {
            tilesetEditors[i].Close();
            tilesetEditors.erase(tilesetEditors.begin() + i);
        }
        else
        {
            tilesetEditors[i].Update();
        }
    }

    // Get mouse.
    mouseX = GetMouseX();
    mouseY = GetMouseY();

    // Panning.
    CheckPan();
    
    // Scrolling.
    CheckScroll();

    // Zooming.
    CheckZoom();

    // Set mouse position.
    oldMouseX = mouseX;
    oldMouseY = mouseY;

    // Update focus.
    focus.Update();

}

void Editor::CheckPan()
{
    if (inPan)
    {
        if (IsMouseButtonUp(MOUSE_MIDDLE_BUTTON))
        {
            inPan = false;
        }
        else
        {
            MoveCamX(mouseX - oldMouseX);
            MoveCamY(mouseY - oldMouseY);
        }
    }
    else if (!focus.mouseInWindow && IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !focus.isModal)
    {
        inPan = true;
    }
}

void Editor::CheckScroll()
{
    if (focus.windowFocused || focus.isModal)
    {
        return;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        cam.offset.y += scrollSpeed;
        //cam.target.y -= scrollSpeed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        cam.offset.y -= scrollSpeed;
        //cam.target.y += scrollSpeed;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        cam.offset.x += scrollSpeed;
        //cam.target.x -= scrollSpeed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        cam.offset.x -= scrollSpeed;
        //cam.target.x += scrollSpeed;
    }
}

void Editor::CheckZoom()
{

    // Focus check.
    if (focus.mouseInWindow || focus.isModal)
    {
        return;
    }

    // Targetting (Make zooming be where mouse is).
    float oldTargX = cam.target.x;
    float oldTargY = cam.target.y;
    cam.target.x = mouseX - oldMouseX;
    cam.target.y = mouseY - oldMouseY;
    cam.offset.x -= (oldTargX - cam.target.x) * cam.zoom;
    cam.offset.y -= (oldTargY - cam.target.y) * cam.zoom;

    // Zooming.
    float zoom = GetMouseWheelMove();
    if (zoom != 0)
    {
        cam.zoom += zoom * zoomSpeed;
        if (cam.zoom <= .5f)
        {
            cam.zoom = .5f;
        }
        else if (cam.zoom > 5)
        {
            cam.zoom = 5;
        }
    }

}