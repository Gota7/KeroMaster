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

    // Draw tile to place. TODO!!!

    // Draw entity to place. TODO!!!

    // Show the grid.
    DrawGrid();
    EndMode2D();

}

void EditorNew::DrawUI()
{

    // Safety.
    if (!enabled) return;

    // Main menu.
    DrawMainMenu();

    // Level editor.
    levelEditor.DrawUI();

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

bool EditorNew::KeyboardShortcut(bool control, bool alt, bool shift, int key)
{
    bool ret = true;
    if (control) ret &= IsKeyDown(KEY_LEFT_CONTROL) | IsKeyDown(KEY_RIGHT_CONTROL);
    if (alt) ret &= IsKeyDown(KEY_LEFT_ALT) | IsKeyDown(KEY_RIGHT_ALT);
    if (shift) ret &= IsKeyDown(KEY_LEFT_SHIFT) | IsKeyDown(KEY_RIGHT_SHIFT);
    ret &= IsKeyPressed(key);
    return ret;
}

void EditorNew::DrawMainMenu()
{

    // Vars.
    static int numFiles;
    static char** files;
    static std::string newFileName = "";
    bool openPopup = false;
    bool openSettings = false;
    bool doNew = KeyboardShortcut(true, false, false, KEY_N);
    bool doOpen = KeyboardShortcut(true, false, false, KEY_O);
    bool doSave = KeyboardShortcut(true, false, false, KEY_S);
    bool doSaveAs = KeyboardShortcut(true, false, true, KEY_S);
    bool doClose = KeyboardShortcut(true, false, true, KEY_C);
    bool doQuit = KeyboardShortcut(true, false, true, KEY_Q);
    bool doUndo = undoStack.CanUndo() && KeyboardShortcut(true, false, false, KEY_Z);
    bool doRedo = undoStack.CanRedo() && (KeyboardShortcut(true, false, false, KEY_Y) || KeyboardShortcut(true, false, true, KEY_Z));
    bool isFullscreening = false;
    bool openNewLevelPopup = false;
    bool openAboutPopup = false;
    static bool isNew = false;

    // File menu.
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                doNew = true;
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                doOpen = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                doSave = true;
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
            {
                doSaveAs = true;
            }
            if (ImGui::MenuItem("Close", "Ctrl+Shift+C"))
            {
                doClose = true;
            }
            if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q"))
            {
                doQuit = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (undoStack.CanUndo() && ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
                doUndo = true;
            }

            if (undoStack.CanRedo() && ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
                doRedo = true;
            }

            if (ImGui::MenuItem("Settings"))
            {
                openSettings = true;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Level Editor"))
            {
                levelEditor.open = true;
            }

            if (ImGui::MenuItem("Profile Editor"))
            {
                //profileEditor.open = true;
            }

            if (ImGui::MenuItem("Style Editor"))
            {
                //styleEditor.open = true;
            }

            if (ImGui::MenuItem("Music Player"))
            {
                //musicPlayer.open = true;
            }
            ImGui::EndMenu();
        }
        /* TODO: TOOL SELECT!!!
        if (!startup && ImGui::BeginMenu("Tool"))
        {
            ImGui::RadioButton("Hand (Ctrl+Q)", (int*)&currentTool, (int)EditorTool::Hand);
            ImGuiTooltip("Pan the camera.");
            ImGui::RadioButton("Tile Brush (Ctrl+W)", (int*)&currentTool, (int)EditorTool::TileBrush);
            ImGuiTooltip("Paint tiles.");
            ImGui::RadioButton("Eraser (Ctrl+E)", (int*)&currentTool, (int)EditorTool::Eraser);
            ImGuiTooltip("Erase tiles.");
            ImGui::RadioButton("Entity Hand (Ctrl+R)", (int*)&currentTool, (int)EditorTool::EntityHand);
            ImGuiTooltip("Move, edit, place, and delete entities.");
            ImGui::EndMenu();
        }*/
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Play Area", &settings.showPlayArea);
            ImGui::Checkbox("Show Grid", &settings.showGrid);
            ImGui::Separator();
            ImGui::Checkbox("Show Foreground Layer", &settings.viewLayers[(u8)MapLayer::FG]);
            ImGui::Checkbox("Show Middleground Layer", &settings.viewLayers[(u8)MapLayer::MG]);
            ImGui::Checkbox("Show Background Layer", &settings.viewLayers[(u8)MapLayer::BG]);
            ImGui::Separator();
            ImGui::Checkbox("Entity Images", &settings.viewEntityImages);
            ImGui::Checkbox("Entity Boxes", &settings.viewEntityBoxes);
            ImGui::Checkbox("Entities", &settings.viewEntities);
            ImGui::Separator();
            ImGui::Checkbox("Tile Attributes", &settings.viewTileAttributes);
            if (ImGui::Button("Fullscreen"))
            {
                isFullscreening = true;
            }
            ImGui::EndMenu();
        }
        /*if (ImGui::MenuItem("Help")) // Help may not be needed.
        {
            helpModal = startup;
            showHelp = true;
        }*/
        if (ImGui::MenuItem("About"))
        {
            openAboutPopup = true;
        }
        ImGui::EndMainMenuBar();
    }

}

void EditorNew::OpenTileset(std::string tilesetName)
{

}

void EditorNew::OpenScript(std::string scriptName)
{

}