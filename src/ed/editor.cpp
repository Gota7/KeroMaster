#include <iostream>
#include <fstream>
#include <imgui.h>
#include "editor.h"
#include "../rlImGui/fileBrowser.h"

std::map<string, Tileset> Editor::tilesets;
std::map<u8, EntityDisplay> Editor::entities;

using namespace imgui_addons;

void Editor::SetPath(string rsc)
{
    this->rsc = rsc;
}

void Editor::LoadEnemies(string xmlName)
{
    entities = LoadXML(xmlName);
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
}

void Editor::UnloadLevel()
{
    map.Unload(tilesets);
}

void Editor::Draw()
{

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

    // Vars.
    static int numFiles;
    static char** files;
    bool openPopup = false;

    // File menu.
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            imguiFocused = true;
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                numFiles = 0;
                files = GetDirectoryFiles((rsc + "/field").c_str(), &numFiles);
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
            imguiFocused = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            imguiFocused = true;
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
            imguiFocused = true;
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("About"))
        {   
            imguiFocused = true;
        }
        ImGui::EndMainMenuBar();
    }

    // Level select.
    if (openPopup)
    {
        ImGui::OpenPopup("Select Level");
        openPopup = false;
    }
    if (ImGui::BeginPopupModal("Select Level"))
    {
        imguiFocused = true;
        ImGui::BeginListBox("Levels", ImVec2(300, 700));
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

    // Safety.
    if (!enabled)
    {
        return;
    }

    // Editor.
    ImGui::Begin(("Level Editor - " + mapName).c_str());
    if (ImGui::IsWindowFocused())
    {
        imguiFocused = true;
    }
    ImGui::End();

}

void Editor::Update()
{

    // Safety.
    if (!enabled)
    {
        imguiFocused = false;
        return;
    }

    // Get mouse.
    mouseX = GetMouseX();
    mouseY = GetMouseY();

    // Middle mouse scroll.
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
    {
        cam.offset.x += (mouseX - oldMouseX);
        cam.offset.y += (mouseY - oldMouseY);
    }

    // Focused.
    if (imguiFocused)
    {
        //imguiFocused = false;
        //oldMouseX = mouseX;
        //oldMouseY = mouseY;
        //return;
    }
    
    // Scrolling.
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

    // Set mouse position.
    oldMouseX = mouseX;
    oldMouseY = mouseY;

}