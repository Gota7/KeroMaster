#include <iostream>
#include <fstream>
#include <imgui.h>
#include "editor.h"
#include "help.h"
#include "../bgm/bgm.h"
#include "../rlImGui/fileBrowser.h"
#include "../rlImGui/utils.h"

std::map<string, Tileset> Editor::tilesets;
std::map<u8, EntityDisplay> Editor::entities;
Settings Editor::settings;
Color Editor::fadeColor = { 255, 0, 0, 255 };
double Editor::timer;
bool Editor::doFullscreen = false;

using namespace imgui_addons;

static char debug_string[1024] = {0};

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
    vector<string> defaultListing = vector<string>();
    fstream f;
    f.open("object_data/unittype.txt", ios::in);
    string s;
    while (getline(f, s))
    {
        defaultListing.push_back(s);
    }
    f.close();
    for (u16 i = 0; i <= 255; i++)
    {
        string toCopy = entities.find(i) != entities.end() ? (to_string(i) + " - " + entities[i].name) : (to_string(i) + " - " + defaultListing[i]);
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
    Tileset::unitType = LoadTexture("object_data/unittype.png");
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
    settings.lastLevel = name;
    settings.Save();
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
    if (viewLayers[2]) map.DrawLayer(2, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewLayers[1]) map.DrawLayer(1, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewLayers[0]) map.DrawLayer(0, tilesets, origin, MAP_SIZE, viewTileAttributes);
    if (viewEntities) map.DrawEntities(entities, tilesets, origin, MAP_SIZE, viewEntityBoxes);

    // Play area.
    if (showPlayArea)
    {
        if (viewLayers[2]) DrawRectangleLinesEx({ 0, 0, map.maps[2].width * MAP_SIZE * 8, map.maps[2].height * MAP_SIZE * 8 }, 3, RED);
        if (viewLayers[1]) DrawRectangleLinesEx({ 0, 0, map.maps[1].width * MAP_SIZE * 8, map.maps[1].height * MAP_SIZE * 8 }, 3, RED);
        if (viewLayers[0]) DrawRectangleLinesEx({ 0, 0, map.maps[0].width * MAP_SIZE * 8, map.maps[0].height * MAP_SIZE * 8 }, 3, RED);
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
        tileDraw = editingTileset->currTile;
        layerDraw = editingTileset->currLayer;
        numTilesX = editingTileset->selectionWidth;
        numTilesY = editingTileset->selectionHeight;
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
                    t.Draw(tileDraw + x + y * t.width, origin, tileX + x, tileY + y, MAP_SIZE, false, false, false, 0, 0, alphaTint);
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
        string tilesetNames[3];
        tilesetNames[0] = map.tilesets[0].dat;
        tilesetNames[1] = map.tilesets[1].dat;
        tilesetNames[2] = map.tilesets[2].dat;
        Str tmp;
        tmp.dat = "";
        if (draw) entities[placeEntityId].Draw(placeEntityId, tmp, 0, true, map.references[RT_NPC_PALETTE].dat, tilesetNames, tilesets, origin, tileX, tileY, MAP_SIZE, viewEntityBoxes);
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

    // Style.
    DrawStyleEditor();

    // Music.
    DrawMusicPlayer();

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
    DrawLevelEditor();

    // Entity editor.
    DrawEntityEditor();

    // Draw palette.
    if (settings.usePalette) DrawPalette();

    // Toolbar.
    DrawToolbar();

    // Profile editor.
    DrawProfileEditor();

}

void Editor::DrawMainMenu(bool startup)
{

    // Vars.
    static int numFiles;
    static char** files;
    static string newFileName = "";
    bool openPopup = false;
    bool openSettings = false;
    bool doNew = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_N);
    bool doOpen = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_O);
    bool doSave = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_S);
    bool doSaveAs = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_S);
    bool doClose = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_C);
    bool doQuit = !startup && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_Q);
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
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
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
                showLevelEditor = true;
            }

            if (ImGui::MenuItem("Profile Editor"))
            {
                showProfileEditor = true;
            }

            if (ImGui::MenuItem("Style Editor"))
            {
                showStyleEditor = true;
            }

            if (ImGui::MenuItem("Music Player"))
            {
                showMusicPlayer = true;
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
    "V0 Half",
    "\0"
};

const char* tileScales[] = {
    "0 (undefined - falls back to 2)",
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
    "16 (1x1)",
    "\0"
};

void Editor::DrawLevelEditor()
{
    if (!showLevelEditor)
    {
        return;
    }

    // Editor.
    auto& io = ImGui::GetIO();

    ImGui::Begin(("Level Editor - " + mapName + "###EditorDialog").c_str(), &showLevelEditor);
    ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - 330.0f, 30.0f), ImGuiCond_FirstUseEver);

    focus.ObserveFocus();
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Title", &map.comment.dat);
    ImGuiTooltip("Internal name for the level.\nThis does not seem important?");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Script", &map.references[RT_SCRIPT].dat);
    ImGuiTooltip("Name of the .pxeve file in the text directory to use for level scripting.");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Next Level", &map.references[RT_NEXT_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears after this level.");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Previous Level", &map.references[RT_PREV_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder that appears before this level.");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Link Level", &map.references[RT_LINK_LEVEL].dat);
    ImGuiTooltip("Name of the .pxpack in the field folder for debug linking.");
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("NPC Palette", &map.references[RT_NPC_PALETTE].dat);
    ImGuiTooltip("Name of the .png in the img folder for NPCs to use (that depend on the NPC palette).");
    ImGui::Separator();
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area X", ImGuiDataType_U16, &map.levelSettings[0]);
    ImGuiTooltip("Start X tile for the area (usually 0).\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Y", ImGuiDataType_U16, &map.levelSettings[2]);
    ImGuiTooltip("Start Y tile for the area (usually 0).\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Number", ImGuiDataType_U8, &map.levelSettings[4]);
    ImGuiTooltip("Which area the level is a part of.\nSeems to have no effect?");
    ImGui::PushItemWidth(itemWidth);

    ImGui::PushItemWidth(itemWidth);
    ImGuiColorEdit("Background Color", &map.levelSettings[5]);
    ImGuiTooltip("Color to display in the background.");

    int tmpScrollType[NUM_TILESETS];
    for (int i = 0 ; i < NUM_TILESETS; i++)
    {
        ImGui::Separator();
        ImGui::PushItemWidth(itemWidth);
        int index = map.tilesetSettings1[i];
        if (ImGui::Combo(("Tileset " + to_string(i) + " Tile Size").c_str(), &index, tileScales, IM_ARRAYSIZE(tileScales)))
        {
            map.tilesetSettings1[i] = (u8)index;
        }
        ImGuiTooltip("Size of how the tiles should be used.");
        tmpScrollType[i] = map.tilesetSettings2[i];
        ImGui::Combo(("Tileset " + to_string(i) + " Scroll Mode").c_str(), &tmpScrollType[i], scrollTypes, IM_ARRAYSIZE(scrollTypes));
        ImGuiTooltip("How to scroll tiles when the camera moves.");
        ImGui::PushItemWidth(itemWidth);
        map.tilesetSettings2[i] = (u8)tmpScrollType[i];
        ImGui::PushItemWidth(itemWidth);
        ImGuiStringEdit(("Tileset " + to_string(i)).c_str(), &map.tilesets[i].dat);
        if (strcmp(map.tilesets[i].dat.c_str(), "") != 0)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton(("Open##Tileset" + to_string(i)).c_str()))
            {
                OpenTileset(map.tilesets[i].dat);
            }
        }
        ImGuiTooltip("Name of the tileset to use in the img folder.");
    }
    for (int i = 0; i < NUM_TILESETS; i++)
    {
        ImGui::Separator();
        ImGui::PushItemWidth(itemWidth / 2 - 5);
        ImGui::InputScalar(("##MapWidth" + to_string(i)).c_str(), ImGuiDataType_U16, &map.maps[i].oldWidth);
        ImGui::SameLine(0, -5.0);
        ImGui::PushItemWidth(itemWidth / 2 - 5);
        ImGui::InputScalar(("Map " + to_string(i) + " Size##MapHeight" + to_string(i)).c_str(), ImGuiDataType_U16, &map.maps[i].oldHeight);
        ImGui::SameLine();
        if (ImGui::SmallButton(("Resize##Map" + to_string(i)).c_str()))
        {
            resizeWarning = false;
            for (int x = map.maps[i].oldWidth; x < map.maps[i].width; x++)
            {
                for (int y = 0; y < map.maps[i].height; y++)
                {
                    if (map.maps[i].GetTile(x, y) != 0)
                    {
                        resizeMapLayer = i;
                        resizeWarning = true;
                        break;
                    }
                }
            }
            for (int y = map.maps[i].oldHeight; y < map.maps[i].height; y++)
            {
                for (int x = 0; x < map.maps[i].width; x++)
                {
                    if (map.maps[i].GetTile(x, y) != 0)
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
                map.maps[i].Resize(map.maps[i].oldWidth, map.maps[i].oldHeight);
            }
        }
        if (ImGui::SmallButton(("Move Up##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(ShiftDirection::Up);
            if (resizeWarning)
            {
                resizeMode = ShiftDirection::Up;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(ShiftDirection::Up);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Down##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(ShiftDirection::Down);
            if (resizeWarning)
            {
                resizeMode = ShiftDirection::Down;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(ShiftDirection::Down);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Left##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(ShiftDirection::Left);
            if (resizeWarning)
            {
                resizeMode = ShiftDirection::Left;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(ShiftDirection::Left);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Right##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(ShiftDirection::Right);
            if (resizeWarning)
            {
                resizeMode = ShiftDirection::Right;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(ShiftDirection::Right);
            }
        }
    }

    int numButtons = 0;
    ImGui::Separator();
    if (strcmp(map.references[RT_SCRIPT].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Script"))
        {
            OpenScript(map.references[RT_SCRIPT].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }

    if (strcmp(map.references[RT_NEXT_LEVEL].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Next Level"))
        {
            LoadLevel(map.references[RT_NEXT_LEVEL].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }

    if (strcmp(map.references[RT_PREV_LEVEL].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Previous Level"))
        {
            LoadLevel(map.references[RT_PREV_LEVEL].dat);
        }
        numButtons++;
        if (numButtons < 2)
        {
            ImGui::SameLine();
        } else {
            numButtons = 0;
        }
    }

    if (strcmp(map.references[RT_LINK_LEVEL].dat.c_str(), "") != 0)
    {
        if (ImGui::Button("Edit Link Level"))
        {
            LoadLevel(map.references[RT_LINK_LEVEL].dat);
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
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            tilesets[map.tilesets[i].dat].Unload();
            tilesets.erase(map.tilesets[i].dat);
        }
        for (int i = 0; i < NUM_TILESETS; i++)
        {
            Tileset t;
            t.Load(rsc, map.tilesets[i].dat);
            tilesets[map.tilesets[i].dat] = t;
        }
    }

    ImGui::End();

    // Warning for resizing.
    if (resizeWarning)
    {
        resizeWarning = false;
        ImGui::OpenPopup("Map Warning");
    }
    if (ImGui::BeginPopupModal("Map Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        focus.ObserveFocus();
        focus.isModal |= true;
        ImGui::Text("This map operation will lose data.\nAre you sure you want to continue?");
        if (ImGui::Button("Yes", ImVec2(itemWidth / 1.33, 0)))
        {
            switch (resizeMode)
            {
                case ShiftDirection::Up:
                case ShiftDirection::Down:
                case ShiftDirection::Left:
                case ShiftDirection::Right:
                    map.maps[resizeMapLayer].Shift(resizeMode);
                    break;
                case ShiftDirection::Resize:
                    map.maps[resizeMapLayer].Resize(map.maps[resizeMapLayer].oldWidth, map.maps[resizeMapLayer].oldHeight);
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

void Editor::DrawEntityEditor()
{
    if (editingEntity == nullptr) return;
    
    ImGui::Begin("Entity Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(10.0f, 80.0f), ImGuiCond_FirstUseEver);

    focus.ObserveFocus();
    int currId = editingEntity->id;
    EntityDisplay* d = &entities[currId];
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    if (ImGui::Combo("Entity Id", &currId, entityListing))
    {
        editingEntity->id = currId;
    }
    if (strcmp(d->description.c_str(), "") != 0)
    {
        ImGuiTooltip(d->description);
    }
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Flags", ImGuiDataType_U8, &editingEntity->flags);
    if (strcmp(d->flagDescription.c_str(), "") != 0)
    {
        ImGuiTooltip(d->flagDescription);
    }
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Unknown", ImGuiDataType_U8, &editingEntity->unk);
    for (int i = 0; i < NUM_BYTE_PARAMETERS; i++)
    {
        ImGui::PushItemWidth(itemWidth);
        ImGui::InputScalar(("Parameter " + to_string(i)).c_str(), ImGuiDataType_U8, &editingEntity->parametersByte[i]);
        if (strcmp(d->parameterDescriptions[i].c_str(), "") != 0)
    {
        ImGuiTooltip(d->parameterDescriptions[i]);
    }
    }
    for (int i = 0; i < NUM_PARAMETERS - NUM_BYTE_PARAMETERS; i++)
    {
        ImGui::PushItemWidth(itemWidth);
        ImGuiStringEdit(("Parameter " + to_string(i + NUM_BYTE_PARAMETERS)).c_str(), &editingEntity->parametersStr[i].dat);
        if (strcmp(d->parameterDescriptions[i + NUM_BYTE_PARAMETERS].c_str(), "") != 0)
        {
            ImGuiTooltip(d->parameterDescriptions[i + NUM_BYTE_PARAMETERS]);
        }
    }
    ImGui::End();
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
        float scale = te->second.textureScale;
        if (scale == 0.0f) scale = 2.0f;
        scale = 1 / scale;

        ImGui::Text("Tile ID: %d\nAttribute: %s\n", currentTile, te->second.GetAttributeName(currentTile));

        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 tmp = ImGui::GetCursorPos();
        ImGui::Image(&te->second.tex, ImVec2(te->second.width * 16, te->second.height * 16), ImVec2(0, 0), ImVec2(scale, scale));
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
            currTileX = min(startTileX, endTileX);
            currTileY = min(startTileY, endTileY);
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
    ImGui::SliderFloat("", &cam.zoom, 0.25f, 5.0f, "Scale: %.2fx", ImGuiSliderFlags_NoRoundToFormat);
    
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
        ImGui::Combo("Place Entity Id", &placeEntityId, entityListing);
        ImGui::SameLine();
        if (ToggleButton("+", isPlacingEntity, nullptr))
        {
            isPlacingEntity = !isPlacingEntity;
        }
        Tooltip("Click after hitting to place an entity or again to cancel.\nHolding shift while placing will allow you to place multiple.\nPressing the delete key will delete the selected entity.");
    }

    ImGui::End();
}

void Editor::DrawProfileEditor()
{
    if (!showProfileEditor)
    {
        return;
    }
    ImGui::Begin("Profile Editor", &showProfileEditor);
    focus.ObserveFocus();
    if (ImGui::BeginTabBar("Profile Tabs"))
    {
        if (ImGui::BeginTabItem("Screen Settings"))
        {
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Editor::DrawStyleEditor()
{
    static bool scanDirs = true;
    static int numThemes = 0;
    static char** themes;
    static char** themes2 = nullptr;
    static ImVector<char*> themeList;
    static int currTheme = 0;
    if (scanDirs)
    { 
        ImVec2 center = ImGui::GetMainViewport()->GetCenter(); // I hate C this should not be this ridiculous.
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        themeList.clear();
        if (themes2 != nullptr)
        {
            free(themes2);
            themes2 = nullptr;
        }
        numThemes = 0;
        themes = GetDirectoryFiles("object_data/themes", &numThemes);
        themes2 = (char**)malloc((numThemes - 2) * sizeof(char*));
        qsort(themes, numThemes, sizeof(char*), cmpstr);
        for (int i = 2; i < numThemes; i++)
        {
            const char* dat = GetFileNameWithoutExt(themes[i]);
            themes2[i - 2] = (char*)malloc((strlen(dat) + 1) * sizeof(char));
            strcpy(themes2[i - 2], dat);
            themeList.push_back(themes2[i - 2]);
            if (strcmp(themeList[i - 2], settings.style.name.c_str()) == 0)
            {
                currTheme = i - 2;
            }
        }
        numThemes -= 2;
        scanDirs = false;
    }
    if (!showStyleEditor) return;
    ImGui::Begin("Style Editor", &showStyleEditor);
    focus.ObserveFocus();

    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    if (ImGui::Combo("Current Style", &currTheme, themeList.begin(), numThemes))
    {
        settings.style.Load(themeList[currTheme]);
        settings.Save();
    }
    string oldName = settings.style.name;
    if (ImGuiStringEdit("Style Name", &settings.style.name))
    {
        settings.style.Save();
        settings.Save();
        scanDirs = true;
        remove(("object_data/themes/" + oldName + ".ini").c_str());
    }
    if (strcmp(settings.style.name.c_str(), "New Theme") && ImGui::Button("New Theme"))
    {
        settings.style.name = "New Theme";
        scanDirs = true;
        settings.style.Save();
        settings.Save();
    }
    if (numThemes > 1)
    {
        if (strcmp(settings.style.name.c_str(), "New Theme") != 0) { ImGui::SameLine(); }
        if (ImGui::Button("Delete Theme"))
        {
            remove(("object_data/themes/" + settings.style.name + ".ini").c_str());
            settings.style.Load(themeList[0]);
            scanDirs = true;
        }
    }
    if (ImGui::Button("Save Style"))
    {
        settings.style.style = ref_saved_style = style;
        settings.style.Save();
    }
    ImGui::SameLine();
    if (ImGui::Button("Revert Style"))
        style = settings.style.style;

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); ImGuiTooltip("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); ImGuiTooltip("Alignment applies when a selectable is larger than its text content.");
            ImGui::Text("Safe Area Padding");
            ImGui::SameLine(); ImGuiTooltip("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            ImGuiTooltip(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &settings.style.style.Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { settings.style.style.Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = settings.style.style.Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();
            ImGuiTooltip("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();
            ImGuiTooltip("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
    ImGui::End();
}

void Editor::DrawMusicPlayer()
{
    if (!showMusicPlayer)
    {
        return;
    }
    ImGui::Begin("Music Player", &showMusicPlayer, ImGuiWindowFlags_AlwaysAutoResize);
    focus.ObserveFocus();
    ImGui::Combo("Song", &BgmPlayer::currSongInd, BgmPlayer::songList.begin(), BgmPlayer::numSongs);
    ImGui::ProgressBar(BgmPlayer::GetPos() / (float)max(BgmPlayer::GetEnd(), 1));
    ImGui::SliderFloat("Volume", &BgmPlayer::volume, 0, 1, "%f", 1);
    if (ImGui::Button("Play"))
    {
        BgmPlayer::Play(BgmPlayer::songList[BgmPlayer::currSongInd]);
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause/Resume"))
    {
        if (BgmPlayer::playing)
        {
            BgmPlayer::Pause();
        }
        else
        {
            BgmPlayer::Resume();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        BgmPlayer::Stop();
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
    auto t = TilesetEditor(this, name);
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
    attrEditors.push_back(AttributeEditor(this, name));
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
            attrEditors[i].target = LoadRenderTexture(tilesets[name].width * 8 * 2 + (float)Tileset::attrTex.width, max((float)tilesets[name].height * 8 * 2, (float)Tileset::attrTex.height));
        }
    }
}

void Editor::RemoveAllOtherTilesetViewerSelections(TilesetEditor* exclude)
{
    currentTile = -1;
    for (int i = 0; i < tilesetEditors.size(); i++)
    {
        if (&tilesetEditors[i] != exclude)
        {
            tilesetEditors[i].currTile = -1;
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

        if (cam.zoom <= .25f)
        {
            cam.zoom = .25f;
        }
        else if (cam.zoom > 5.0f)
        {
            cam.zoom = 5.0f;
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
        placeTile = editingTileset->currTile;
        placeLayer = editingTileset->currLayer;
        placeW = editingTileset->selectionWidth;
        placeH = editingTileset->selectionHeight;
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
                if (editingEntity != nullptr)
                {
                    editingEntity->beingEdited = false;
                }
                editingEntity = &map.entities[i];
                map.entities[i].beingEdited = true;
                found = true;
                break;
            }
        }
        if (!found && editingEntity != nullptr)
        {
            editingEntity->beingEdited = false;
            editingEntity = nullptr;
        }
    }
    else if (!isPlacingEntity && editingEntity != nullptr && ((settings.ButtonDown(EditorTool::CurrentTool) && currentTool == EditorTool::EntityHand) || settings.ButtonDown(EditorTool::EntityHand)) && !focus.mouseInWindow)
    {
        int tileX = GetTileX();
        int tileY = GetTileY();
        if (tileX < 0) { tileX = 0; }
        if (tileY < 0) { tileY = 0; }
        if (tileX > 0xFFFF) { tileX = 0xFFFF; }
        if (tileY > 0xFFFF) { tileY = 0xFFFF; }
        editingEntity->xPos = tileX;
        editingEntity->yPos = tileY;
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
    }
    if (editingEntity != nullptr && IsKeyPressed(KEY_DELETE))
    {
        editingEntity->beingEdited = false;
        Entity* e = editingEntity;
        editingEntity = nullptr;
        for (int i = 0; i < map.entities.size(); i++)
        {
            if (&map.entities[i] == e)
            {
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