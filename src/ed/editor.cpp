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
    //editingEntity = &map.entities[0];
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
    for (int i = 0; i < attrEditors.size(); i++)
    {
        attrEditors[i].Draw();
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

    // Safety.
    if (!enabled)
    {
        return;
    }

    // Level editor.
    DrawLevelEditor();

    // Entity editor.
    DrawEntityEditor();

    DrawPalette();

    // Toolbar.
    DrawToolbar();
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
                map.Write(rsc, mapName);
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
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Play Area", &showPlayArea);
            ImGui::Checkbox("Show Grid", &showGrid);
            ImGui::Separator();
            ImGui::Checkbox("Show Foreground layer", &viewLayers[2]);
            ImGui::Checkbox("Show Middleground layer", &viewLayers[1]);
            ImGui::Checkbox("Show Background layer", &viewLayers[0]);
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
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndListBox();
        ImGui::EndPopup();
    }

}

static const std::vector<const char*> scrollTypes = {
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
};

void Editor::DrawLevelEditor()
{
    // Editor.
    auto& io = ImGui::GetIO();

    ImGui::Begin(("Level Editor - " + mapName + "###EditorDialog").c_str());
    ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - 330.0f, 30.0f), ImGuiCond_FirstUseEver);

    focus.ObserveFocus();
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Title", &map.comment.dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Level Script", &map.references[RT_SCRIPT].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Next Level", &map.references[RT_NEXT_LEVEL].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Previous Level", &map.references[RT_PREV_LEVEL].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Link Level", &map.references[RT_LINK_LEVEL].dat);
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("NPC Palette", &map.references[RT_NPC_PALETTE].dat);
    ImGui::Separator();
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area X", ImGuiDataType_U16, &map.levelSettings[0]);
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Y", ImGuiDataType_U16, &map.levelSettings[2]);
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Area Number", ImGuiDataType_U8, &map.levelSettings[4]);
    ImGui::PushItemWidth(itemWidth);

    ImGui::PushItemWidth(itemWidth);
    ImGuiColorEdit("Background Color", &map.levelSettings[5]);

    int tmpScrollType[NUM_TILESETS];
    for (int i = 0 ; i < NUM_TILESETS; i++)
    {
        ImGui::Separator();
        ImGui::PushItemWidth(itemWidth);
        int index = map.tilesetSettings1[i];
        if (ImGui::Combo(("Tileset " + to_string(i) + " Tile Size").c_str(), &index, "Nothing\00016x16\0008x8\0004x4\0002x2\0001x1\0"))
        {
            map.tilesetSettings1[i] = (u8)index;
        }
        tmpScrollType[i] = map.tilesetSettings2[i];
        ImGui::Combo(("Tileset " + to_string(i) + " Scroll Mode").c_str(), &tmpScrollType[i], scrollTypes.data(), scrollTypes.size());
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
                        resizeMode = SHIFT_RESIZE;
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
            resizeWarning = !map.maps[i].CanShift(SHIFT_UP);
            if (resizeWarning)
            {
                resizeMode = SHIFT_UP;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(SHIFT_UP);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Down##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(SHIFT_DOWN);
            if (resizeWarning)
            {
                resizeMode = SHIFT_DOWN;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(SHIFT_DOWN);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Left##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(SHIFT_LEFT);
            if (resizeWarning)
            {
                resizeMode = SHIFT_LEFT;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(SHIFT_LEFT);
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("Move Right##Map" + to_string(i)).c_str()))
        {
            resizeWarning = !map.maps[i].CanShift(SHIFT_RIGHT);
            if (resizeWarning)
            {
                resizeMode = SHIFT_RIGHT;
                resizeMapLayer = i;
            }
            else
            {
                map.maps[i].Shift(SHIFT_RIGHT);
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
                case SHIFT_UP:
                case SHIFT_DOWN:
                case SHIFT_LEFT:
                case SHIFT_RIGHT:
                    map.maps[resizeMapLayer].Shift(resizeMode);
                    break;
                case SHIFT_RESIZE:
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

void Editor::DrawPalette() 
{
    if (currentTool != EditorTool::TileBrush) return;

    ImGui::Begin("Palette", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(10.0f, 80.0f), ImGuiCond_FirstUseEver);
    focus.ObserveFocus();

    auto te = &tilesetEditors[currentLayer];
    if (te->open) {
        ImGui::Image(&te->target.texture, ImVec2(256, 256));
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

    ImGui::SameLine();
    ImGui::SliderFloat("", &cam.zoom, 0.25f, 5.0f, "Scale: %.2fx", ImGuiSliderFlags_NoRoundToFormat);
    
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &showGrid);

    if (ToggleButton("FG", currentLayer == 0, &viewLayers[0])) {
        currentLayer = 0;
        viewLayers[0] = true;
    }
    Tooltip("Foreground layer\nRight click to toggle visibility.");

    if (ToggleButton("MG", currentLayer == 1, &viewLayers[1])) {
        currentLayer = 1;
        viewLayers[1] = true;
    }
    Tooltip("Middleground layer\nRight click to toggle visibility.");

    if (ToggleButton("BG", currentLayer == 2, &viewLayers[2])) {
        currentLayer = 2;
        viewLayers[2] = true;
    }
    Tooltip("Background layer\nRight click to toggle visibility.");

    ImGui::End();
}

void Editor::DrawProfileEditor()
{

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
    tilesetEditors.push_back(TilesetEditor(this, name));
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

void Editor::CheckPan()
{
    if (inPan)
    {
        if (IsMouseButtonUp(MOUSE_RIGHT_BUTTON) && (currentTool == EditorTool::Hand && IsMouseButtonUp(MOUSE_LEFT_BUTTON)))
        {
            inPan = false;
        }
        else
        {
            MoveCamX(mouseX - oldMouseX);
            MoveCamY(mouseY - oldMouseY);
        }
    }
    else if (!focus.mouseInWindow && (
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) 
            || (currentTool == EditorTool::Hand && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        ) && !focus.isModal)
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
        if (cam.zoom < 1.0f) {
            zoomSpeed = 0.125f;
        } else if (cam.zoom < 3.0f) {
            zoomSpeed = 0.25f;
        } else {
            zoomSpeed = 0.5f;
        }

        cam.zoom += zoom * zoomSpeed;
        //cam.zoom = ((int) (cam.zoom / zoomSpeed)) * zoomSpeed;

        if (cam.zoom <= .25f)
        {
            cam.zoom = .25f;
        }
        else if (cam.zoom > 8.0f)
        {
            cam.zoom = 8.0f;
        }
    }

}