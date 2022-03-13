#include "editorNew.h"
#include "../px/tileset.h"
#include "../rlImGui/rlImGui.h"
#include "../rlImGui/utils.h"
#include "imgui.h"
#include "raylib.h"

void EditorNew::Init()
{

    // Load the settings.
    settings.Load();

    // Then set up the main window.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | (settings.maximized ? FLAG_WINDOW_MAXIMIZED : 0));
    InitWindow(settings.width, settings.height, "Kero Master");
    icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    SetupRLImGui(true);
    style.Load(settings.currStyle);

    // Initialize editor.
    InitEditor();

}

int EditorNew::EditorLoop()
{

    // Main loop.
    while (!WindowShouldClose())
    {

        // Begin drawing.
        BeginDrawing();

        // Something went wrong, need to show the settings window.
        if (settings.show)
        {
            ClearBackground(fadeColor);
            BeginRLImGui();
            ImGui::OpenPopup("Editor Settings");
            settings.DrawUI(this);
        }

        // Draw like normal.
        else
        {
            Draw();
            BeginRLImGui();
            DrawUI();
        }

        // ImGui demo debug.
        //ImGui::ShowDemoWindow();
        
        // Finish drawing.
        EndRLImGui();
        EndDrawing();
        Update();

        // Fullscreen check.
        /*if (IsKeyPressed(KEY_F4) || doFullscreen)
        {
            if (!IsWindowFullscreen() && !doFullscreen)
            {
                MaximizeWindow();
                timer = GetTime();
                doFullscreen = true;
            }
            if (GetTime() > e.timer + 0.1)
            {
                ToggleFullscreen();
                doFullscreen = false;
            }
        }*/

    }

    // Unload everything.
    UnloadImage(icon);
    ShutdownRLImGui();
    CloseWindow();

    // Nothing wrong, return.
    return 0;

}

void EditorNew::Draw()
{

    // Draw the sub-editors.

    // Safety.
    if (!enabled) return;

    // Draw background.
    map.Clear();
    BeginMode2D(cam);

    // Draw map.
    if (settings.viewLayers[(u8)MapLayer::BG]) map.DrawLayer((u8)MapLayer::BG, tilesets, { 0, 0 }, settings.viewTileAttributes);
    if (settings.viewLayers[(u8)MapLayer::MG]) map.DrawLayer((u8)MapLayer::MG, tilesets, { 0, 0 }, settings.viewTileAttributes);
    if (settings.viewLayers[(u8)MapLayer::FG]) map.DrawLayer((u8)MapLayer::FG, tilesets, { 0, 0 }, settings.viewTileAttributes);

    // Show the map play area.
    if (settings.showPlayArea)
    {
        if (settings.viewLayers[(u8)MapLayer::BG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::BG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::BG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
        if (settings.viewLayers[(u8)MapLayer::MG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::MG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::MG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
        if (settings.viewLayers[(u8)MapLayer::FG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::FG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::FG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
    }

    // Show the grid.
    DrawGrid();

}

void EditorNew::DrawUI()
{

}

void EditorNew::Update()
{
    
    // Safety, can't use the editor here.
    if (!enabled)
    {
        FadeEffect();
        return;
    }

}

void EditorNew::FadeEffect()
{
    Vector3 col = ColorToHSV(fadeColor);
    col.x += 0.25;
    if (col.x >= 360)
    {
        col.x = 0;
    }
    fadeColor = ColorFromHSV(col.x, col.y, col.z);
}

void EditorNew::InitEditor()
{
    entityEditor.LoadEntityListing("all");
    LoadFixedTilesets();
    if (settings.rscPath != rsc || settings.lastLevel != level)
    {
        rsc = settings.rscPath;
        level = settings.lastLevel;
        LoadLevel();
    }
}

void EditorNew::LoadTileset(std::string tilesetName)
{
    Tileset t;
    t.Load(rsc, tilesetName);
    tilesets[tilesetName] = t;
}

void EditorNew::UnloadTileset(std::string tilesetName)
{
    if (tilesets.find(tilesetName) != tilesets.end())
    {
        tilesets[tilesetName].Unload();
    }
}

void EditorNew::LoadFixedTilesets()
{
    std::fstream f;
    f.open("object_data/alwaysLoaded.txt", std::ios::in);
    std::string s;
    while (std::getline(f, s))
    {
        LoadTileset(s);
    }
    f.close();
    Tileset::attrTex = LoadTexture("object_data/attribute.png");
    Tileset::unitType = LoadTexture("object_data/unittype.png");
}

void EditorNew::LoadLevel()
{
    if (enabled)
    {
        map.Unload(tilesets);
    }
    cam.offset = { 0, 0 };
    cam.rotation = 0;
    cam.target = { 0, 0 };
    cam.zoom = 2;
    map.Load(rsc, level, tilesets);
    enabled = true;
    settings.lastLevel = level;
    settings.Save();
    undoStack.Reset();
}

void EditorNew::UnloadLevel()
{
    map.Unload(tilesets);
}

void EditorNew::DrawGrid()
{
    if (settings.showGrid)
    {
        int gridWidth = 0;
        int gridHeight = 0;
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            if (settings.viewLayers[i])
            {
                if (map.maps[i].width > gridWidth)
                {
                    gridWidth = map.maps[i].width;
                }
                if (map.maps[i].height > gridHeight)
                {
                    gridHeight = map.maps[i].height;
                }
            }
        }
        const float GRID_ALPHA = 0.1;
        for (int i = 0; i < gridWidth; i++)
        {
            for (int j = 0; j < gridHeight; j++)
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
                DrawRectangle(i * Tileset::EDITOR_TILE_SIZE, j * Tileset::EDITOR_TILE_SIZE, Tileset::EDITOR_TILE_SIZE, Tileset::EDITOR_TILE_SIZE, c);
            }
        }
    }
}