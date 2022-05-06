#include "editor.h"
#include "attributeEditor.h"
#include "entityDisplay.h"
#include "entityEditor.h"
#include "help.h"
#include "levelEditor.h"
#include "musicPlayer.h"
#include "profileEditor.h"
#include "scriptEditor.h"
#include "settings.h"
#include "styleEditor.h"
#include "tilesetEditor.h"
#include "undoStack.h"
#include "tools/hand.h"
#include "../bgm/bgm.h"
#include "../px/pxmap.h"
#include "../px/tileset.h"
#include "../rlImGui/fileBrowser.h"
#include "../rlImGui/utils.h"
#include <iostream>
#include <fstream>
#include <imgui.h>

std::map<std::string, Tileset> Editor::tilesets;
std::map<u8, EntityDisplay> Editor::entities;
Settings Editor::settings;
Color Editor::fadeColor = { 255, 0, 0, 255 };
double Editor::timer;
bool Editor::doFullscreen = false;
const float MIN_ZOOM = 0.25;
const float MAX_ZOOM = 10.0;
//std::vector<Tool> Editor::tools = { HandTool() };

using namespace imgui_addons;

static char debug_string[1024] = {0};

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa, bb);
}

void Editor::Init()
{
    //currTool = &tools[0];
}

void Editor::SetPath(std::string rsc)
{
    this->rsc = rsc;
}

void Editor::LoadEnemies(std::string xmlName)
{
    entities = LoadXML(xmlName);
    //if (!entityEditor) entityEditor = new EntityEditor(this);
    //entityEditor->LoadEntityListing();
}

void Editor::LoadTileset(std::string tilesetName)
{
    Tileset t;
    t.Load(rsc, tilesetName);
    tilesets[tilesetName] = t;
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

void Editor::LoadLevel(std::string name)
{
    if (enabled)
    {
        map.Unload(tilesets);
    }
    mapName = name;
    cam.offset = { 0, 0 };
    cam.rotation = 0;
    cam.target = { 0, 0 };
    cam.zoom = 2;
    origin = { 0, 0 };
    map.Load(rsc, name, tilesets);
    enabled = true;
    for (int i = 0; i < NUM_TILESETS; i++)
    {
        if (strcmp(map.tilesets[i].dat.c_str(), "") != 0)
        {
            if (openTilesetsOnLoad) { tilesetEditors.push_back(TilesetEditor(this, map.tilesets[i].dat, 8)); }
        }
    }
    settings.lastLevel = name;
    settings.Save();
    undoStack->Reset();
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

int Editor::GetTileX(s8 layer)
{
    int ret = ((mouseX - cam.offset.x) / (MAP_SIZE * 8) / cam.zoom);
    if (ret < 0 || (layer != -1 && ret >= map.maps[layer].width))
    {
        return -1;
    }
    else
    {
        return ret;
    }
}

int Editor::GetTileY(s8 layer)
{
    int ret = ((mouseY - cam.offset.y) / (MAP_SIZE * 8) / cam.zoom);
    if (ret < 0 || (layer != -1 && ret >= map.maps[layer].width))
    {
        return -1;
    }
    else
    {
        return ret;
    }
}

void Editor::Draw()
{

    // Secondaries.
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        tilesetEditors[i].Draw();
    }
    for (int i = 0; i < attrEditors.size(); i++)
    {
        attrEditors[i].Draw();
    }

    // Safety.
    if (!enabled)
    {
        ClearBackground(fadeColor);
        return;
    }

    // BG.
    map.Clear();

    // Push.
    BeginMode2D(cam);

    // Draw map.
    if (viewLayers[2]) map.DrawLayer(2, tilesets, origin, viewTileAttributes);
    if (viewLayers[1]) map.DrawLayer(1, tilesets, origin, viewTileAttributes);
    if (viewLayers[0]) map.DrawLayer(0, tilesets, origin, viewTileAttributes);
    if (viewEntities) map.DrawEntities(entities, tilesets, origin, viewEntityBoxes);

    // Play area.
    if (showPlayArea)
    {
        if (viewLayers[2]) DrawRectangleLinesEx({ 0, 0, map.maps[2].width * MAP_SIZE * 8, map.maps[2].height * MAP_SIZE * 8 }, 1, RED);
        if (viewLayers[1]) DrawRectangleLinesEx({ 0, 0, map.maps[1].width * MAP_SIZE * 8, map.maps[1].height * MAP_SIZE * 8 }, 1, RED);
        if (viewLayers[0]) DrawRectangleLinesEx({ 0, 0, map.maps[0].width * MAP_SIZE * 8, map.maps[0].height * MAP_SIZE * 8 }, 1, RED);
    }

    // Grid.
    if (showGrid)
    {
        int gridWidth = 0;
        int gridHeight = 0;
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            if (viewLayers[i])
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
                DrawRectangle(i * 8 * MAP_SIZE, j * 8 * MAP_SIZE, 8 * MAP_SIZE, 8 * MAP_SIZE, c);
            }
        }
    }

    int tileDraw = currentTile;
    int layerDraw = currentLayer;
    int numTilesX = selectionWidth;
    int numTilesY = selectionHeight;
    if (tileDraw == -1 && editingTileset != nullptr)
    {
        tileDraw = editingTileset->selectedTile;
        layerDraw = editingTileset->currLayer;
        numTilesX = editingTileset->selection.width;
        numTilesY = editingTileset->selection.height;
    }
    if (currentTool == EditorTool::TileBrush && tileDraw != -1)
    {
        int tileX = GetTileX();
        int tileY = GetTileY();
        static Color alphaTint = ColorFromNormalized({ 1.0, 1.0, 1.0, 0.6 });

        PxMap& m = map.maps[layerDraw];
        Tileset& t = tilesets[map.tilesets[layerDraw].dat];

        for (int x = 0; x < numTilesX; x++)
        {
            for (int y = 0; y < numTilesY; y++)
            {
                if (x >= 0 && y >= 0 & tileX + x < m.width && tileY + y < m.height)
                {
                    t.Draw(tileDraw + x + y * t.width, tileX + x, tileY + y, map.TileSize(layerDraw), Tileset::EDITOR_TILE_SIZE, false, false, false, { 0, 0 }, alphaTint);
                }
            }
        }
    }

    if (isPlacingEntity)
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
            entities[placeEntityId].Draw(
                &e,
                tilesets,
                map.references[RT_NPC_PALETTE].dat,
                tilesetNames,
                false,
                origin
            );
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
    for (int i = 0; i < attrEditors.size(); i++)
    {
        attrEditors[i].DrawUI();
    }
    for (int i = 0; i < scriptEditors.size(); i++)
    {
        scriptEditors[i].DrawUI();
    }

    // Style editor.
    //if (!styleEditor) styleEditor = new StyleEditor(this);
    styleEditor->DrawUI();

    // Music.
    //if (!musicPlayer) musicPlayer = new MusicPlayer(this);
    musicPlayer->DrawUI();

    // Help.
    if (showHelp)
    {
        ShowHelp(&focus, &showHelp, helpModal);
    }

    // Safety.
    if (!enabled)
    {
        return;
    }

    // Level editor.
    //if (!levelEditor) levelEditor = new LevelEditor(this);
    levelEditor->DrawUI();

    // Entity editor.
    //if (!entityEditor) entityEditor = new EntityEditor(this);
    //entityEditor->DrawUI();

    // Draw palette.
    if (settings.usePalette) DrawPalette();

    // Toolbar.
    DrawToolbar();

    // Profile editor.
    if (!profileEditor)
    {
        //profileEditor = new ProfileEditor(this);
        profile.ReadAll(rsc);
    }
    profileEditor->DrawUI();

}

void Editor::DrawMainMenu(bool startup)
{

    // Vars.
    static int numFiles;
    static char** files;
    static std::string newFileName = "";
    bool openPopup = false;
    bool openSettings = false;
    bool doNew = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_N);
    bool doOpen = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_O);
    bool doSave = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_S);
    bool doSaveAs = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_S);
    bool doClose = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_C);
    bool doQuit = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_Q);
    bool doUndo = !startup && undoStack->CanUndo() && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_Z);
    bool doRedo = !startup && undoStack->CanRedo() && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_Z)) || IsKeyPressed(KEY_Y));
    if (!startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_Q)) { currentTool = EditorTool::Hand; }
    if (!startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_W)) { currentTool = EditorTool::TileBrush; }
    if (!startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_E)) { currentTool = EditorTool::Eraser; }
    if (!startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_R)) { currentTool = EditorTool::EntityHand; }
    bool isFullscreening = false;
    bool openNewLevelPopup = false;
    bool openAboutPopup = false;
    static bool isNew = false;

    // File menu.
    if (startup ? ImGui::BeginMenuBar() : ImGui::BeginMainMenuBar())
    {
        if (!startup)
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
        }
        if (!startup && ImGui::BeginMenu("Edit"))
        {
            if (undoStack->CanUndo() && ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
                doUndo = true;
            }

            if (undoStack->CanRedo() && ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
                doRedo = true;
            }

            if (ImGui::MenuItem("Settings"))
            {
                openSettings = true;
            }

            ImGui::EndMenu();
        }
        if (!startup && ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Level Editor"))
            {
                levelEditor->open = true;
            }

            if (ImGui::MenuItem("Profile Editor"))
            {
                profileEditor->open = true;
            }

            if (ImGui::MenuItem("Style Editor"))
            {
                styleEditor->open = true;
            }

            if (ImGui::MenuItem("Music Player"))
            {
                musicPlayer->open = true;
            }
            ImGui::EndMenu();
        }
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
        }
        if (!startup && ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Play Area", &showPlayArea);
            ImGui::Checkbox("Show Grid", &showGrid);
            ImGui::Separator();
            ImGui::Checkbox("Show Foreground Layer", &viewLayers[0]);
            ImGui::Checkbox("Show Middleground Layer", &viewLayers[1]);
            ImGui::Checkbox("Show Background Layer", &viewLayers[2]);
            ImGui::Separator();
            ImGui::Checkbox("Entity Boxes", &viewEntityBoxes);
            ImGui::Checkbox("Entities", &viewEntities);
            ImGui::Separator();
            ImGui::Checkbox("Tile Attributes", &viewTileAttributes);
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

        ImGui::Text("%s", debug_string);

        if (startup) { ImGui::EndMenuBar(); } else { ImGui::EndMainMenuBar(); }
    }

    // Options.
    if (doNew)
    {
        openNewLevelPopup = true;
        isNew = true;
    }
    else if (doOpen)
    {
        numFiles = 0;
        files = GetDirectoryFiles((rsc + "/field").c_str(), &numFiles);
        qsort(files, numFiles, sizeof(char*), cmpstr);
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        openPopup = true;
    }
    else if (doSaveAs)
    {
        isNew = false;
        openNewLevelPopup = true;
    }
    else if (doSave)
    {
        if (enabled)
        {
            map.Write(rsc, mapName);
        }
    }
    else if (doClose)
    {
        enabled = false;
        map.Unload(tilesets);
    }
    else if (doQuit)
    {
        exit(0);
    }
    else if (doRedo)
    {
        undoStack->Redo(this);
    }
    else if (doUndo)
    {
        undoStack->Undo(this);
    }
    else if (isFullscreening || doFullscreen)
    {
        if (!IsWindowFullscreen() && !doFullscreen)
        {
            MaximizeWindow();
            timer = GetTime();
            doFullscreen = true;
        }
        if (GetTime() > timer + 0.1)
        {
            ToggleFullscreen();
            doFullscreen = false;
        }
    }

    // Settings.
    if (openSettings)
    {
        ImGui::OpenPopup("Editor Settings");
        openSettings = false;
    }
    settings.ShowWindow(this);

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

    // About.
    if (openAboutPopup)
    {
        ImGui::OpenPopup("About");
        openAboutPopup = false;
    }
    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGui::TextColored(ImVec4(.4, 1, 0, 1), "Kero Master");
        ImGui::TextColored(ImVec4(1, .1, .5, 1), "\tAn editor for Kero Blaster, Pink Hour, and Pink Heaven.");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Alula - Windows & SHIFT-JIS support, tile editing, palette, various fixes/improvements.");
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "Gota7 - UI design, format support, entity editing, script editing, tileset editing, editor data.");
        ImGui::Separator();
        if (ImGui::Button("Ok")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // New level.
    static bool closeNewLevel;
    if (openNewLevelPopup)
    {
        ImGui::OpenPopup("Enter Level Name");
        newFileName = "";
        openNewLevelPopup = false;
        closeNewLevel = false;
    }
    if (ImGui::BeginPopupModal("Enter Level Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGuiStringEdit("Level Name", &newFileName);
        if (strcmp(newFileName.c_str(), "") != 0)
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
                    LoadLevel(newFileName);
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
                        UnloadLevel();
                        LoadLevel(newFileName);
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
        if (strcmp(newFileName.c_str(), "") != 0)
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

void Editor::DrawPalette() 
{
    if (currentTool != EditorTool::TileBrush) return;

    ImGui::Begin("Palette", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(10.0f, 80.0f), ImGuiCond_FirstUseEver);
    focus.ObserveFocus();
    ImGui::Checkbox("Show attributes", &showPaletteAttributes);
    ImGui::Dummy(ImVec2(0, 4));

    auto te = tilesets.find(map.tilesets[currentLayer].dat);
    if (te != tilesets.end())
    {
        float scale = map.tilesetSettings1[currentLayer];
        if (scale == 0.0f) scale = 2.0f;
        scale = 1 / scale;

        ImGui::Text("Tile ID: %d\nAttribute: %s\n", currentTile, te->second.GetAttributeName(currentTile));

        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 tmp = ImGui::GetCursorPos();
        ImGui::Image(&te->second.tex, ImVec2(te->second.width * 16, te->second.height * 16), ImVec2(0, 0), ImVec2((float)te->second.width * 8 / te->second.tex.width, (float)te->second.height * 8 / te->second.tex.height));
        ImGui::SetCursorPos(tmp);
        ImGui::InvisibleButton("NoDrag", ImVec2(te->second.width * 16, te->second.height * 16));
        if (showPaletteAttributes) {
            for (int i = 0; i < te->second.width * 16; i++) {
                int attr = te->second.GetTilesetAttr(i);
                int tileX = i % 16;
                int tileY = i / 16;
                int attrX = attr % 16;
                int attrY = attr / 16;

                ImGui::SetCursorScreenPos(ImVec2(p.x + tileX * 16, p.y + tileY * 16));
                const ImVec2 uv1 = ImVec2(attrX / 16.0f, attrY / 16.0f);
                const ImVec2 uv2 = ImVec2(uv1.x + 1/16.0f, uv1.y + 1/16.0f);
                
                ImGui::Image(&Tileset::attrTex, ImVec2(16, 16), uv1, uv2);
            }
        }

        auto drawList = ImGui::GetWindowDrawList();
        auto cursorPos = ImVec2(
            p.x + 16 * (currentTile % 16), 
            p.y + 16 * (currentTile / 16)
        );
        if (currentTile != -1) drawList->AddRect(cursorPos, ImVec2(cursorPos.x + 16 * selectionWidth, cursorPos.y + 16 * selectionHeight), ImColor(255, 255, 255), 0, 0, 2);

        const ImVec2 mousePos = ImGui::GetMousePos();
        const ImVec2 mousePosRel = ImVec2(mousePos.x - p.x, mousePos.y - p.y);
        if (currentTool == EditorTool::TileBrush && ImGui::IsMouseDown(MOUSE_LEFT_BUTTON) && !isSelecting && mousePosRel.x >= 0 && mousePosRel.x < te->second.width * 16 &&
                mousePosRel.y >= 0 && mousePosRel.y < te->second.height * 16)
        {
            isSelecting = true;
            int tileX = selectionStartX = mousePosRel.x / 16;
            int tileY = selectionStartY = mousePosRel.y / 16;
            selectionWidth = 1;
            selectionHeight = 1;
            RemoveAllOtherTilesetViewerSelections(nullptr);
            currentTile = tileY * te->second.width + tileX;
        }
        else if (isSelecting && currentTool == EditorTool::TileBrush && ImGui::IsMouseReleased(MOUSE_LEFT_BUTTON))
        {
            isSelecting = false;
        }
        else if (isSelecting)
        {
            int startTileX = selectionStartX;
            int startTileY = selectionStartY;
            int endTileX = mousePosRel.x / 16;
            int endTileY = mousePosRel.y / 16;
            if (endTileX >= te->second.width)
            {
                endTileX = te->second.width - 1;
            }
            else if (endTileX < 0)
            {
                endTileX = 0;
            }
            if (endTileY >= te->second.height)
            {
                endTileY = te->second.height - 1;
            }
            else if (endTileY < 0)
            {
                endTileY = 0;
            }
            int currTileX;
            int currTileY;
            currTileX = std::min(startTileX, endTileX);
            currTileY = std::min(startTileY, endTileY);
            selectionWidth = abs(endTileX - startTileX) + 1;
            selectionHeight = abs(endTileY - startTileY) + 1;
            currentTile = currTileX + currTileY * te->second.width;
        }
    }

    ImGui::End();
}

void Editor::DrawToolbar() 
{
    ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::SetWindowPos(ImVec2(10.0f, 30.0f), ImGuiCond_FirstUseEver);

    focus.ObserveFocus();

    int index = 0;
    auto ToolButton = [&](const char* label, EditorTool tool) {
        bool active = currentTool == tool;

        if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        
        if (index++ > 0) ImGui::SameLine();
        if (ImGui::Button(label)) {
            currentTool = tool;
        }
        
        if (active) ImGui::PopStyleColor();
    };

    auto ToggleButton = [&](const char* label, bool active, bool* rightToggle) -> bool {
        bool action = false;
        if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        
        if (index++ > 0) ImGui::SameLine();
        if (ImGui::Button(label)) {
            action = true;
        } else if (ImGui::IsItemClicked(1)) {
            if (rightToggle != nullptr) {
                *rightToggle = !*rightToggle;
            }
        }
        
        if (active) ImGui::PopStyleColor();
        return action;
    };

    auto Tooltip = [](const char* message) {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::SetTooltip("%s", message);
            ImGui::EndTooltip();
        }
    };

    ToolButton("Hand", EditorTool::Hand);
    ToolButton("Tile Brush", EditorTool::TileBrush);
    ToolButton("Eraser", EditorTool::Eraser);
    ToolButton("Entity Hand", EditorTool::EntityHand);

    ImGui::SameLine();
    float bakZoom = cam.zoom;
    ImGui::SliderFloat("", &cam.zoom, MIN_ZOOM, MAX_ZOOM, "Scale: %.2fx", ImGuiSliderFlags_NoRoundToFormat);
    if (cam.zoom != bakZoom)
    {
        cam.offset.x = -(mouseX - cam.offset.x) * cam.zoom / bakZoom + mouseX;
        cam.offset.y = -(mouseY - cam.offset.y) * cam.zoom / bakZoom + mouseY;
    }
    
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &showGrid);

    if (ToggleButton("FG", currentLayer == 0, &viewLayers[0])) {
        currentLayer = 0;
        viewLayers[0] = true;
    }
    Tooltip("Foreground Layer\nRight click to toggle visibility.");

    if (ToggleButton("MG", currentLayer == 1, &viewLayers[1])) {
        currentLayer = 1;
        viewLayers[1] = true;
    }
    Tooltip("Middleground Layer\nRight click to toggle visibility.");

    if (ToggleButton("BG", currentLayer == 2, &viewLayers[2])) {
        currentLayer = 2;
        viewLayers[2] = true;
    }
    Tooltip("Background Layer\nRight click to toggle visibility.");
    if (currentTool == EditorTool::EntityHand)
    {
        ImGui::SameLine();
        ImGui::Combo("Place Entity Id", &placeEntityId, entityEditor->entityListing, entityEditor->numEntities);
        ImGui::SameLine();
        if (ToggleButton("+", isPlacingEntity, nullptr))
        {
            isPlacingEntity = !isPlacingEntity;
        }
        Tooltip("Click after hitting to place an entity or again to cancel.\nHolding shift while placing will allow you to place multiple.\nPressing the delete key will delete the selected entity.");
    }

    ImGui::End();
}

void Editor::OpenTileset(std::string name)
{
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (strcmp(tilesetEditors[i].name.c_str(), name.c_str()) == 0)
        {
            return;
        }
    }
    auto t = TilesetEditor(this, name, 8);
    if (strcmp(name.c_str(), map.tilesets[2].dat.c_str()) == 0) { t.allowLayer2 = true; t.currLayer = 2; }
    if (strcmp(name.c_str(), map.tilesets[1].dat.c_str()) == 0) { t.allowLayer1 = true; t.currLayer = 1; }
    if (strcmp(name.c_str(), map.tilesets[0].dat.c_str()) == 0) { t.allowLayer0 = true; t.currLayer = 0; }
    tilesetEditors.push_back(t);
}

void Editor::OpenAttrEditor(std::string name)
{
    for (int i = 0; i < attrEditors.size(); i++)
    {
        if (strcmp(attrEditors[i].name.c_str(), name.c_str()) == 0)
        {
            return;
        }
    }
    attrEditors.push_back(AttributeEditor(this, name, 8.0f));
}

void Editor::OpenScript(std::string name)
{
    for (int i = 0; i < scriptEditors.size(); i++)
    {
        if (strcmp(scriptEditors[i].name.c_str(), name.c_str()) == 0)
        {
            return;
        }
    }
    scriptEditors.push_back(ScriptEditor(this, name));
}

void Editor::Update()
{

    // Safety.
    if (!enabled)
    {
        UpdateFadeColor();
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
    mouseX = GetMouseX();
    mouseY = GetMouseY();

    // Panning.
    CheckPan();
    /*std::vector<int> handButtons;
    if (currentTool == EditorTool::Hand)
    {
        handButtons.push_back(MOUSE_BUTTON_LEFT);
    }
    handButtons.push_back(MOUSE_BUTTON_RIGHT);
    tools[0].SetActive(this, handButtons);
    tools[0].Update(this);*/
    
    // Scrolling.
    CheckScroll();

    // Zooming.
    CheckZoom();

    // Check for editing.
    CheckEdit();

    // Check entity.
    CheckEntity();

    // Set mouse position.
    oldMouseX = mouseX;
    oldMouseY = mouseY;

    // Update focus.
    focus.Update();

}

void Editor::ResizeAllTilesetViewers(std::string name)
{
    if (tilesets.find(name) == tilesets.end())
    {
        return;
    }
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (tilesetEditors[i].open && strcmp(tilesetEditors[i].name.c_str(), name.c_str()) == 0)
        {
            UnloadRenderTexture(tilesetEditors[i].target);
            tilesetEditors[i].target = LoadRenderTexture(tilesets[name].width * 8 * 2, tilesets[name].height * 8 * 2);
        }
    }
    for (int i = 0; i < attrEditors.size(); i++)
    {
        if (attrEditors[i].open && strcmp(attrEditors[i].name.c_str(), name.c_str()) == 0)
        {
            UnloadRenderTexture(attrEditors[i].target);
            attrEditors[i].target = LoadRenderTexture(tilesets[name].width * 8 * 2 + (float)Tileset::attrTex.width, std::max((float)tilesets[name].height * 8 * 2, (float)Tileset::attrTex.height));
        }
    }
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
}

void Editor::CheckPan()
{
    if (inPan)
    {
        dragLeft = currentTool == EditorTool::Hand && !settings.ButtonUp(EditorTool::CurrentTool);
        dragRight = !settings.ButtonUp(EditorTool::Hand);
        inPan = dragLeft || dragRight;

        MoveCamX(mouseX - oldMouseX);
        MoveCamY(mouseY - oldMouseY);
    } else if (!focus.mouseInWindow && !focus.isModal)
    {
        dragLeft = currentTool == EditorTool::Hand && settings.ButtonPressed(EditorTool::CurrentTool);
        dragRight = settings.ButtonPressed(EditorTool::Hand);
        inPan = dragLeft || dragRight;
    }
}

void Editor::CheckScroll()
{
    if (focus.windowFocused || focus.isModal)
    {
        return;
    }

    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)))
    {
        cam.offset.y += scrollSpeed;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)))
    {
        cam.offset.y -= scrollSpeed;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
    {
        cam.offset.x += scrollSpeed;
    }
    if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT) && (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
    {
        cam.offset.x -= scrollSpeed;
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
        float beforeZoom = cam.zoom;
        if (cam.zoom < 1.0f) {
            zoomSpeed = 0.125f;
        } else if (cam.zoom < 3.0f) {
            zoomSpeed = 0.25f;
        } else {
            zoomSpeed = 0.5f;
        }

        cam.zoom += zoom * zoomSpeed;

        if (cam.zoom <= MIN_ZOOM)
        {
            cam.zoom = MIN_ZOOM;
        }
        else if (cam.zoom > MAX_ZOOM)
        {
            cam.zoom = MAX_ZOOM;
        }

        // So the mouse before zooming must be the mouse after, or: (mouse - offBefore) / zoomBefore = (mouse - offAfter) / zoomAfter.
        // I solved for the offAfter variable here.
        cam.offset.x = -(mouseX - cam.offset.x) * cam.zoom / beforeZoom + mouseX;
        cam.offset.y = -(mouseY - cam.offset.y) * cam.zoom / beforeZoom + mouseY;
        
    }
    
}

void Editor::CheckEdit()
{
    if (focus.mouseInWindow || focus.isModal)
    {
        return;
    }

    int placeLayer = currentLayer;
    int tileX = ((mouseX - cam.offset.x) / (MAP_SIZE * 8) / cam.zoom);
    int tileY = ((mouseY - cam.offset.y) / (MAP_SIZE * 8) / cam.zoom);
    int placeTile = currentTile;
    int placeW = selectionWidth;
    int placeH = selectionHeight;
    if (placeTile == -1 && editingTileset != nullptr)
    {
        placeTile = editingTileset->selectedTile;
        placeLayer = editingTileset->currLayer;
        placeW = editingTileset->selection.width;
        placeH = editingTileset->selection.height;
    }
    PxMap& layer = map.maps[placeLayer];
    if (placeTile != -1 && tileX >= 0 && tileX < layer.width && tileY >= 0 && tileY < layer.height && !isSelecting) {
        if ((currentTool == EditorTool::TileBrush && settings.ButtonDown(EditorTool::CurrentTool)) || settings.ButtonDown(EditorTool::TileBrush)) {
            for (int x = 0; x < placeW; x++)
            {
                for (int y = 0; y < placeH; y++)
                {
                    if (tileX + x >= 0 && tileY + y >= 0)
                    {
                        layer.SetTile(tileX + x, tileY + y, placeTile + x + y * tilesets[map.tilesets[placeLayer].dat].width);
                    }
                }
            }
        }
        else if ((currentTool == EditorTool::Eraser && settings.ButtonDown(EditorTool::CurrentTool)) || settings.ButtonDown(EditorTool::Eraser)) {
            layer.SetTile(tileX, tileY, 0);
        }
    }
}

void Editor::CheckEntity()
{
    if (!isPlacingEntity && ((settings.ButtonPressed(EditorTool::CurrentTool) && currentTool == EditorTool::EntityHand) || settings.ButtonPressed(EditorTool::EntityHand)) && !focus.mouseInWindow)
    {
        int tileX = GetTileX();
        int tileY = GetTileY();
        bool found = false;
        for (int i = 0; i < map.entities.size(); i++)
        {
            if (tileX == map.entities[i].xPos && tileY == map.entities[i].yPos)
            {
                if (entityEditor->editingEntity != nullptr)
                {
                    entityEditor->editingEntity->beingEdited = false;
                }
                entityEditor->editingEntity = &map.entities[i];
                map.entities[i].beingEdited = true;
                found = true;
                break;
            }
        }
        if (!found && entityEditor->editingEntity != nullptr)
        {
            entityEditor->editingEntity->beingEdited = false;
            entityEditor->editingEntity = nullptr;
        }
    }
    else if (!isPlacingEntity && entityEditor->editingEntity != nullptr && ((settings.ButtonDown(EditorTool::CurrentTool) && currentTool == EditorTool::EntityHand) || settings.ButtonDown(EditorTool::EntityHand)) && !focus.mouseInWindow)
    {
        if (!doingEntityMove)
        {
            backupEntityX = entityEditor->editingEntity->xPos;
            backupEntityY = entityEditor->editingEntity->yPos;
        }
        int tileX = GetTileX();
        int tileY = GetTileY();
        if (tileX < 0) { tileX = 0; }
        if (tileY < 0) { tileY = 0; }
        if (tileX > 0xFFFF) { tileX = 0xFFFF; }
        if (tileY > 0xFFFF) { tileY = 0xFFFF; }
        entityEditor->editingEntity->xPos = tileX;
        entityEditor->editingEntity->yPos = tileY;
        doingEntityMove = true;
    }
    else if (isPlacingEntity && ((settings.ButtonPressed(EditorTool::CurrentTool) && currentTool == EditorTool::EntityHand) || settings.ButtonPressed(EditorTool::EntityHand)))
    {
        int tileX = GetTileX();
        int tileY = GetTileY();
        if (tileX < 0) { return; }
        if (tileY < 0) { return; }
        if (tileX > 0xFFFF) { return; }
        if (tileY > 0xFFFF) { return; }
        Entity e = Entity();
        e.beingEdited = false;
        e.flags = 1;
        e.id = placeEntityId;
        e.parametersByte[0] = 0;
        e.parametersByte[1] = 0;
        e.parametersStr[0].dat = "";
        e.unk = 0;
        e.xPos = tileX;
        e.yPos = tileY;
        map.entities.push_back(e);
        if (IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT))
        {
            isPlacingEntity = false;
        }
        undoStack->PushEntityPlaced(this, map.entities.size() - 1, e.id, e.xPos, e.yPos);
    }
    else if (!isPlacingEntity && doingEntityMove && entityEditor->editingEntity != NULL)
    {
        doingEntityMove = false;
        for (int i = 0; i < map.entities.size(); i++)
        {
            if (entityEditor->editingEntity == &map.entities[i] && (backupEntityX != entityEditor->editingEntity->xPos || backupEntityY != entityEditor->editingEntity->yPos))
            {
                undoStack->PushEntityMoved(this, i, backupEntityX, backupEntityY, entityEditor->editingEntity->xPos, entityEditor->editingEntity->yPos);
            }
        }
    }
    if (entityEditor->editingEntity != nullptr && IsKeyPressed(KEY_DELETE))
    {
        entityEditor->editingEntity->beingEdited = false;
        Entity* e = entityEditor->editingEntity;
        entityEditor->editingEntity = nullptr;
        for (int i = 0; i < map.entities.size(); i++)
        {
            if (&map.entities[i] == e)
            {
                undoStack->PushEntityDeleted(this, i, map.entities[i]);
                map.entities.erase(map.entities.begin() + i);
            }
        }
    }
}

void Editor::UpdateFadeColor()
{
    Vector3 col = ColorToHSV(fadeColor);
    col.x += 0.25;
    if (col.x >= 360)
    {
        col.x = 0;
    }
    fadeColor = ColorFromHSV(col.x, col.y, col.z);
}