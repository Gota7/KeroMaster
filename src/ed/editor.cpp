#include "editor.h"
#include "../bgm/bgm.h"
#include "../px/tileset.h"
#include "../rlImGui/rlImGui.h"
#include "../rlImGui/utils.h"
#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

void Editor::Init()
{

    // Load the settings.
    settings.Load();

    // Then set up the main window.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | (settings.maximized ? FLAG_WINDOW_MAXIMIZED : 0));
    InitWindow(settings.width, settings.height, "Kero Master");
    SetExitKey(0);
    icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    SetupRLImGui(true);
    style.Load(settings.currStyle);

    // Initialize editor.
    InitEditor();

}

int Editor::EditorLoop()
{

    // Main loop.
    while (!WindowShouldClose() && !quit)
    {

        // Begin drawing.
        BeginDrawing();

        // Something went wrong, need to show the settings window.
        if (settings.show || !enabled)
        {
            if (!settings.show) settings.show = true;
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
            settings.DrawUI(this);
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

void Editor::Draw()
{

    // Safety.
    if (!enabled) return;

    // Draw palette.
    palette.Draw();

    // Draw the sub-editors.
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        tilesetEditors[i].Draw();
    }
    for (int i = 0; i < attrEditors.size(); i++)
    {
        attrEditors[i].Draw();
    }

    // Draw background.
    map.Clear();
    BeginMode2D(cam);

    // Draw map.
    if (settings.viewLayers[(u8)MapLayer::BG]) map.DrawLayer((u8)MapLayer::BG, tilesets, { 0, 0 }, settings.viewTileAttributes);
    if (settings.viewLayers[(u8)MapLayer::MG]) map.DrawLayer((u8)MapLayer::MG, tilesets, { 0, 0 }, settings.viewTileAttributes);
    if (settings.viewLayers[(u8)MapLayer::FG]) map.DrawLayer((u8)MapLayer::FG, tilesets, { 0, 0 }, settings.viewTileAttributes);

    // Draw entities.
    if (settings.viewEntities) map.DrawEntities(entityEditor.entities, tilesets, { 0, 0 }, settings.viewEntityBoxes);

    // Show the map play area.
    if (settings.showPlayArea)
    {
        if (settings.viewLayers[(u8)MapLayer::BG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::BG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::BG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
        if (settings.viewLayers[(u8)MapLayer::MG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::MG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::MG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
        if (settings.viewLayers[(u8)MapLayer::FG]) DrawRectangleLinesEx({ 0, 0, map.maps[(u8)MapLayer::FG].width * Tileset::EDITOR_TILE_SIZE, map.maps[(u8)MapLayer::FG].height * Tileset::EDITOR_TILE_SIZE }, 1, RED);
    }

    // Draw tile to place.
    DrawTilePlacePreview();

    // Draw entity to place.
    DrawEntityPlacePreview();

    // Show the grid.
    DrawGrid();
    EndMode2D();

}

void Editor::DrawTilePlacePreview()
{
    if (currTool == ToolItem::TileBrush && tilesToPaint.validSelection)
    {
        int tileX = GetTileX();
        int tileY = GetTileY();
        static Color alphaTint = ColorFromNormalized({ 1.0, 1.0, 1.0, 0.6 });

        PxMap& m = map.maps[tilesToPaint.layer];
        Tileset& t = tilesets[map.tilesets[tilesToPaint.layer].dat];

        for (int x = 0; x < tilesToPaint.width; x++)
        {
            for (int y = 0; y < tilesToPaint.height; y++)
            {
                if (x >= 0 && y >= 0 & tileX + x < m.width && tileY + y < m.height)
                {
                    t.Draw(tilesToPaint.x + x, tilesToPaint.y + y, tileX + x, tileY + y, map.TileSize(tilesToPaint.layer), Tileset::EDITOR_TILE_SIZE, false, false, false, { 0, 0 }, alphaTint);
                }
            }
        }
    }
    else if (currTool == ToolItem::Eraser)
    {
        Tileset& t = tilesets[map.tilesets[currentLayer].dat];
        DrawRectangleRec(
            t.GetDestRect(GetTileX(), GetTileY(), Tileset::EDITOR_TILE_SIZE, { 0, 0 }),
            { map.levelSettings[5], map.levelSettings[6], map.levelSettings[7], 255 }
        );
    }
}

void Editor::DrawEntityPlacePreview()
{
    if (currTool == ToolItem::EntityHand && isPlacingEntity)
    {
        bool draw = true;
        int tileX = GetTileX();
        int tileY = GetTileY();
        if (tileX < 0) { draw = false; }
        if (tileY < 0) { draw = false; }
        if (tileX > 0xFFFF) { draw = false; }
        if (tileY > 0xFFFF) { draw = false; }
        std::string tilesetNames[3];
        tilesetNames[0] = map.tilesets[0].dat;
        tilesetNames[1] = map.tilesets[1].dat;
        tilesetNames[2] = map.tilesets[2].dat;
        if (draw)
        {
            Entity e;
            e.beingEdited = false;
            e.flags = 0;
            e.id = placeEntityId;
            e.parametersByte[0] = 0;
            e.parametersByte[1] = 0;
            e.parametersStr[0].dat = "";
            e.unk = 0;
            e.xPos = tileX;
            e.yPos = tileY;
            entityEditor.entities[placeEntityId].Draw(
                &e,
                tilesets,
                map.references[RT_NPC_PALETTE].dat,
                tilesetNames,
                true,
                { 0, 0 }
            );
        }
    }
}

void Editor::DrawUI()
{

    // Main menu.
    DrawMainMenu();

    // Toolbar gadgets.
    toolbar.DrawUI();

    // Palette.
    palette.DrawUI();

    // Other windows.
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        tilesetEditors[i].DrawUI();
    }
    for (int i = 0; i < attrEditors.size(); i++)
    {
        attrEditors[i].DrawUI();
    }
    for (int i = 0; i < scriptEditors.size(); i++)
    {
        scriptEditors[i].DrawUI();
    }

    // Misc. windows.
    entityEditor.DrawUI();
    levelEditor.DrawUI();
    profileEditor.DrawUI();
    styleEditor.DrawUI();
    musicPlayer.DrawUI();

    // Draw select level UI.
    DrawSelectLevelUI();

    // Draw the open level UI.
    DrawOpenLevelUI();

    // About popup.
    DrawAboutPopup();

}

void Editor::Update()
{

    // Safety, can't use the editor here.
    if (!enabled)
    {
        FadeEffect();
        return;
    }

    // Palette.
    palette.Update();

    // Other editors.
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
    for (int i = attrEditors.size() - 1; i >= 0; i--)
    {
        if (!attrEditors[i].open)
        {
            attrEditors[i].Close();
            attrEditors.erase(attrEditors.begin() + i);
        }
        else
        {
            attrEditors[i].Update();
        }
    }
    for (int i = scriptEditors.size() - 1; i >= 0; i--)
    {
        if (!scriptEditors[i].open)
        {
            scriptEditors[i].Close();
            scriptEditors.erase(scriptEditors.begin() + i);
        }
    }

    // Get mouse.
    oldMouseX = mouseX;
    oldMouseY = mouseY;
    mouseX = GetMouseX();
    mouseY = GetMouseY();

    // Update tools.
    for (int i = 0; i < (int)ToolItem::NumTools; i++)
    {
        ToolItem item = (ToolItem)i;
        std::vector<int> buttons;
        if (settings.leftClick == item || (currTool == item && settings.leftClick == ToolItem::CurrentTool))
            buttons.push_back(MOUSE_LEFT_BUTTON);
        if (settings.middleClick == item || (currTool == item && settings.middleClick == ToolItem::CurrentTool))
            buttons.push_back(MOUSE_MIDDLE_BUTTON);
        if (settings.rightClick == item || (currTool == item && settings.rightClick == ToolItem::CurrentTool))
            buttons.push_back(MOUSE_RIGHT_BUTTON);
        tools.SetActive(this, i, buttons);
    }
    tools.Update(this);

    // Do default tool routine for things like panning and zooming.
    DoDefaultToolRoutine();

    // Update focus.
    focus.Update();

}

void Editor::FadeEffect()
{
    Vector3 col = ColorToHSV(fadeColor);
    col.x += 0.25;
    if (col.x >= 360)
    {
        col.x = 0;
    }
    fadeColor = ColorFromHSV(col.x, col.y, col.z);
}

void Editor::InitEditor()
{
    if (settings.rscPath != rsc || settings.lastLevel != level || levelClosed)
    {
        entityEditor.LoadEntityListing("all");
        for (auto& t : tilesets)
        {
            UnloadTileset(t.first);
        }
        rsc = settings.rscPath;
        level = settings.lastLevel;
        levelClosed = false;
        LoadFixedTilesets();
        LoadLevel();
        InitSubeditors();
    }
    if (settings.maxUndoDepth != undoStack.MaxLen())
    {
        undoStack.SetMaxLen(settings.maxUndoDepth);
    }
}

void Editor::InitSubeditors()
{
    profile.ReadAll(rsc);
    BgmPlayer::Init(rsc);
}

void Editor::LoadTileset(std::string tilesetName)
{
    Tileset t;
    t.Load(rsc, tilesetName);
    tilesets[tilesetName] = t;
}

void Editor::UnloadTileset(std::string tilesetName)
{
    if (tilesets.find(tilesetName) != tilesets.end())
    {
        tilesets[tilesetName].Unload();
    }
}

void Editor::LoadFixedTilesets()
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

void Editor::LoadLevel()
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
    palette.ChangeTileset(map.tilesets[currentLayer].dat);
    if (settings.openTilesetsOnLoad)
    {
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            if (map.tilesets[i].dat != "")
            {
                OpenTileset(map.tilesets[i].dat, 8.0f); // TODO: MAKE THIS OPEN THE CORRECT SIZE!!!
            }
        }
    }
}

void Editor::UnloadLevel()
{
    map.Unload(tilesets);
}

void Editor::DrawGrid()
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

bool Editor::KeyboardShortcut(bool control, bool alt, bool shift, int key)
{
    bool ret = true;
    ret &= !(control ^ (IsKeyDown(KEY_LEFT_CONTROL) | IsKeyDown(KEY_RIGHT_CONTROL)));
    ret &= !(alt ^ (IsKeyDown(KEY_LEFT_ALT) | IsKeyDown(KEY_RIGHT_ALT)));
    ret &= !(shift ^ (IsKeyDown(KEY_LEFT_SHIFT) | IsKeyDown(KEY_RIGHT_SHIFT)));
    ret &= IsKeyPressed(key);
    return ret;
}

void Editor::DrawMainMenu()
{

    // Vars.
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
                profileEditor.open = true;
            }

            if (ImGui::MenuItem("Style Editor"))
            {
                styleEditor.open = true;
            }

            if (ImGui::MenuItem("Music Player"))
            {
                musicPlayer.open = true;
                BgmPlayer::LoadSongList();
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
            if (ImGui::Checkbox("Play Area", &settings.showPlayArea)) settings.Save();
            if (ImGui::Checkbox("Show Grid", &settings.showGrid)) settings.Save();
            ImGui::Separator();
            if (ImGui::Checkbox("Show Foreground Layer", &settings.viewLayers[(u8)MapLayer::FG])) settings.Save();
            if (ImGui::Checkbox("Show Middleground Layer", &settings.viewLayers[(u8)MapLayer::MG])) settings.Save();
            if (ImGui::Checkbox("Show Background Layer", &settings.viewLayers[(u8)MapLayer::BG])) settings.Save();
            ImGui::Separator();
            if (ImGui::Checkbox("Entity Images", &settings.viewEntityImages)) settings.Save();
            if (ImGui::Checkbox("Entity Boxes", &settings.viewEntityBoxes)) settings.Save();
            if (ImGui::Checkbox("Entities", &settings.viewEntities)) settings.Save();
            ImGui::Separator();
            if (ImGui::Checkbox("Tile Attributes", &settings.viewTileAttributes)) settings.Save();
            /*if (ImGui::Button("Fullscreen"))
            {
                isFullscreening = true;
            }*/
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

    // Take care of actions.
    if (doNew) LevelNameSelect(false);
    if (doOpen) LevelNameOpen();
    if (doSave) SaveLevel();
    if (doSaveAs) LevelNameSelect(true);
    if (doClose) CloseLevel();
    if (doQuit) Quit();
    if (doUndo) Undo();
    if (doRedo) Redo();
    if (isFullscreening) DoToggleFullscreen();
    if (openAboutPopup) OpenAboutPopup();

    // Settings.
    if (openSettings)
    {
        ImGui::OpenPopup("Editor Settings");
        openSettings = false;
        settings.softShow = true;
    }

    // Update focus for menu.
    focus.mouseInWindow |= ImGui::IsAnyItemHovered();

}

void Editor::LevelNameSelect(bool saveAs)
{
    newFileName = "";
    closeNewLevel = false;
    isNew = !saveAs;
    ImGui::OpenPopup("Enter Level Name");
}

void Editor::LevelNameOpen()
{

    // Get new level list and then open popup.
    std::vector<std::string> files = ReadFilesFromDir(rsc + "/field");
    std::sort(files.begin(), files.end());
    if (levelFiles != nullptr) DelImGuiStringList(levelFiles, numLevelFiles);
    levelFiles = GenImGuiStringList(files, &numLevelFiles);
    ImGui::OpenPopup("Select Level");

}

void Editor::DrawSelectLevelUI()
{
    if (ImGui::BeginPopupModal("Enter Level Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGuiStringEdit("Level Name", &newFileName);
        if (newFileName != "")
        {
            if (closeNewLevel)
            {
                ImGui::CloseCurrentPopup();
            }
            static Map m;
            if (ImGui::Button("Save"))
            {
                m.levelSettings[0] = 0;
                m.levelSettings[1] = 0;
                m.levelSettings[2] = 0;
                m.levelSettings[3] = 0;
                m.levelSettings[4] = 1;
                m.levelSettings[5] = 0;
                m.levelSettings[6] = 0;
                m.levelSettings[7] = 0;
                m.tilesetSettings1[0] = 2;
                m.tilesetSettings1[1] = 2;
                m.tilesetSettings1[2] = 2;
                m.tilesetSettings2[0] = 0;
                m.tilesetSettings2[1] = 0;
                m.tilesetSettings2[2] = 0;
                bool needsVerify = GFile::FileExists((rsc + "/field/" + newFileName + ".pxpack").c_str());
                if (!needsVerify)
                {
                    if (isNew)
                    {
                        m.Write(rsc, newFileName);
                    }
                    else
                    {
                        map.Write(rsc, newFileName);
                    }
                    UnloadLevel();
                    OpenLevel(newFileName);
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::OpenPopup("Overwrite Level");
                }
            }
            if (ImGui::BeginPopupModal("Overwrite Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::Button("Yes"))
                {
                    if (isNew)
                    {
                        m.Write(rsc, newFileName);
                    }
                    else
                    {
                        map.Write(rsc, newFileName);
                    }
                    OpenLevel(newFileName);
                    ImGui::CloseCurrentPopup();
                    closeNewLevel = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        if (newFileName != "")
        {
            ImGui::SameLine();
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::DrawOpenLevelUI()
{
    if (ImGui::BeginPopupModal("Select Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGui::BeginListBox("Levels", ImVec2(300, 500));
        for (int i = 0; i < numLevelFiles; i++)
        {
            bool dummy = false;
            const char* name = GetFileNameWithoutExt(levelFiles[i]);
            if (ImGui::Selectable(name, &dummy))
            {
                OpenLevel(name);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndListBox();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::DrawAboutPopup()
{
    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGui::TextColored(
            ImVec4(.4, 1, 0, 1),
            (
                "Kero Master - Version " +
                std::to_string(settings.versionMajor) +
                "." + std::to_string(settings.versionMinor) +
                "." + std::to_string(settings.versionRevision)
            ).c_str()
        );
        ImGui::TextColored(ImVec4(1, .1, .5, 1), "\tAn editor for Kero Blaster, Pink Hour, and Pink Heaven.");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Alula - Windows & SHIFT-JIS support, tile editing, palette, various fixes/improvements.");
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "Gota7 - UI design, format support, entity editing, script editing, tileset editing, editor data.");
        ImGui::Separator();
        if (ImGui::Button("Ok"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::OpenLevel(std::string level)
{
    this->level = level;
    LoadLevel();
}

void Editor::SaveLevel()
{
    if (enabled) map.Write(rsc, level);
}

void Editor::CloseLevel()
{
    enabled = false;
    levelClosed = true;
    map.Unload(tilesets);
}

void Editor::Quit()
{
    enabled = false;
    map.Unload(tilesets);
    quit = true;
}

void Editor::Undo()
{
    if (enabled) undoStack.Undo(this);
}

void Editor::Redo()
{
    if (enabled) undoStack.Redo(this);
}

void Editor::DoToggleFullscreen()
{
    ToggleFullscreen();
    if (IsWindowFullscreen())
    {
        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
    }
    else
    {
        SetWindowSize(settings.width, settings.height);
    }
}

void Editor::OpenAboutPopup()
{
    ImGui::OpenPopup("About");
}

void Editor::OpenTileset(std::string tilesetName, float tileSize)
{
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (tilesetEditors[i].name == tilesetName) return; // No duplicate tilesets.
    }
    auto t = TilesetEditor(this, tilesetName, tileSize);
    if (tilesetName == map.tilesets[2].dat) { t.allowLayer2 = true; t.currLayer = 2; }
    if (tilesetName == map.tilesets[1].dat) { t.allowLayer1 = true; t.currLayer = 1; }
    if (tilesetName == map.tilesets[0].dat) { t.allowLayer0 = true; t.currLayer = 0; }
    tilesetEditors.push_back(t);
}

void Editor::RemoveAllOtherTilesetViewerSelections(TilesetEditor* exclude)
{
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (&tilesetEditors[i] != exclude)
        {
            tilesetEditors[i].selection.ClearSelection();
            tilesetEditors[i].selectedTile = -1;
        }
    }
    if (exclude)
    {
        palette.selection.ClearSelection();
        palette.selectedTile = -1;
    }
}

void Editor::ResizeAllTilesetViewers(std::string name)
{
    if (tilesets.find(name) == tilesets.end())
    {
        return;
    }
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (tilesetEditors[i].open && tilesetEditors[i].name == name)
        {
            UnloadRenderTexture(tilesetEditors[i].target);
            tilesetEditors[i].target = LoadRenderTexture(tilesets[name].width * 8 * 2, tilesets[name].height * 8 * 2);
        }
    }
    for (int i = 0; i < attrEditors.size(); i++)
    {
        if (attrEditors[i].open && attrEditors[i].name == name)
        {
            UnloadRenderTexture(attrEditors[i].target);
            attrEditors[i].target = LoadRenderTexture(tilesets[name].width * 8 * 2 + (float)Tileset::attrTex.width, std::max((float)tilesets[name].height * 8 * 2, (float)Tileset::attrTex.height));
        }
    }
}

void Editor::OpenAttr(std::string name, float tileSize)
{
    for (int i = 0; i < attrEditors.size(); i++)
    {
        if (attrEditors[i].name == name) return; // No duplicate editors.
    }
    attrEditors.push_back(AttributeEditor(this, name, tileSize));
}

void Editor::OpenScript(std::string scriptName)
{
    for (int i = 0; i < scriptEditors.size(); i++)
    {
        if (scriptEditors[i].name == scriptName) return; // No duplicate editors.
    }
    scriptEditors.push_back(ScriptEditor(this, scriptName));
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

int Editor::GetTileX(s8 layer)
{
    int ret = ((mouseX - cam.offset.x) / Tileset::EDITOR_TILE_SIZE / cam.zoom);
    if (ret < 0 || (layer != -1 && ret >= map.maps[layer].width))
    {
        return -1;
    }
    else
    {
        return ret;
    }
}

float Editor::GetZoomSpeed()
{
    if (cam.zoom < 1.0f) {
        return 0.125f;
    } else if (cam.zoom < 3.0f) {
        return 0.25f;
    } else {
        return 0.5f;
    }
}

void Editor::Zoom(Vector2 origin, float amount, bool relative)
{

    // Add to existing zoom if relative.
    if (relative)
    {
        amount += cam.zoom;
    }

    // Apply new zoom amount.
    if (amount < MIN_ZOOM) amount = MIN_ZOOM;
    if (amount > MAX_ZOOM) amount = MAX_ZOOM;
    cam.offset.x = -(origin.x - cam.offset.x) * amount / cam.zoom + origin.x;
    cam.offset.y = -(origin.y - cam.offset.y) * amount / cam.zoom + origin.y;
    cam.zoom = amount;

}

int Editor::GetTileY(s8 layer)
{
    int ret = ((mouseY - cam.offset.y) / Tileset::EDITOR_TILE_SIZE / cam.zoom);
    if (ret < 0 || (layer != -1 && ret >= map.maps[layer].width))
    {
        return -1;
    }
    else
    {
        return ret;
    }
}

void Editor::DoDefaultToolRoutine()
{
    CheckScroll();
    CheckZoom();
    CheckEntityDelete();
}

void Editor::CheckScroll()
{
    const float SCROLL_SPEED = 7.5f;
    if (focus.windowFocused || focus.isModal)
    {
        return;
    }

    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_ALT) && IsKeyUp(KEY_RIGHT_ALT) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)))
    {
        cam.offset.y += SCROLL_SPEED;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_ALT) && IsKeyUp(KEY_RIGHT_ALT) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)))
    {
        cam.offset.y -= SCROLL_SPEED;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_ALT) && IsKeyUp(KEY_RIGHT_ALT) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
    {
        cam.offset.x += SCROLL_SPEED;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_ALT) && IsKeyUp(KEY_RIGHT_ALT) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
    {
        cam.offset.x -= SCROLL_SPEED;
    }
}

void Editor::CheckZoom()
{

    // Focus check.
    if (focus.mouseInWindow || focus.isModal)
    {
        return;
    }

    // Zooming.
    float zoom = GetMouseWheelMove();
    if (zoom != 0)
    {
        Zoom({ mouseX, mouseY }, GetZoomSpeed() * zoom / fabs(zoom), true);
    }

}

void Editor::CheckEntityDelete()
{
    if (entityEditor.editingEntity && (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)))
    {
        entityEditor.editingEntity->beingEdited = false;
        Entity* e = entityEditor.editingEntity;
        entityEditor.editingEntity = nullptr;
        for (int i = 0; i < map.entities.size(); i++)
        {
            if (&map.entities[i] == e)
            {
                undoStack.PushEntityDeleted(this, i, map.entities[i]);
                map.entities.erase(map.entities.begin() + i);
            }
        }
    }
}