#include "attributeEditor.h"
#include "editor.h"
#include "../px/tileset.h"
#include "../rlImGui/rlImGui.h"

AttributeEditor::AttributeEditor(Editor* ed, std::string name, float tileSize)
{
    open = true;
    this->ed = ed;
    this->name = name;
    this->tileSize = tileSize;
    if (ed->tilesets.find(name) == ed->tilesets.end()) // Tileset not loaded, close.
    {
        open = false;
    }
    else if (ed->tilesets[name].tiles == nullptr)
    {
        ed->tilesets[name].tiles = new u8[ed->tilesets[name].width * ed->tilesets[name].height]; // Init new tileset attributes.
        memset(ed->tilesets[name].tiles, 0, ed->tilesets[name].width * ed->tilesets[name].height);
    }
    target = LoadRenderTexture(
        ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE + (float)Tileset::attrTex.width,
        std::max((float)ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE, (float)Tileset::attrTex.height)
    ); // Height is just the bigger of the two.
    finalTarget = LoadRenderTexture(target.texture.width, target.texture.height);
}

void AttributeEditor::Draw()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Draw the textures.
    BeginTextureMode(target);
    ClearBackground(BLACK);
    DrawTexturePro(
        ed->tilesets[name].tex,
        { 0, 0, (float)ed->tilesets[name].width * tileSize, (float)ed->tilesets[name].height * tileSize },
        { 0, 0, (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, (float)ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE },
        { 0, 0 },
        0,
        WHITE
    );
    DrawTextureRec(
        Tileset::attrTex,
        { 0, 0, (float)Tileset::attrTex.width, (float)Tileset::attrTex.height },
        { (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, 0 },
        WHITE
    );
    
    // Divider border.
    DrawLineEx(
        { (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, 0 },
        { (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, (float)target.texture.height },
        3,
        RED
    );
    
    // Draw the selected attribute.
    u16 x = attrTile % (int)Tileset::ATTR_TILE_SIZE;
    u16 y = attrTile / Tileset::ATTR_TILE_SIZE;
    DrawRectangleLinesEx(Tileset::GetDestRect(x, y, Tileset::EDITOR_TILE_SIZE, { (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, 0 }), 1, ColorAlpha(YELLOW, .5f));
    
    // Draw each regular attribute.
    for (int i = 0; i < ed->tilesets[name].width; i++)
    {
        for (int j = 0; j < ed->tilesets[name].height; j++)
        {
            Tileset::DrawTileAttr(ed->tilesets[name].GetTilesetAttr(i, j), i, j, Tileset::EDITOR_TILE_SIZE);
        }
    }
    EndTextureMode();
    
    // Hack, flip the Y coordinate by drawing it again.
    BeginTextureMode(finalTarget);
    DrawTexture(target.texture, 0, 0, WHITE);
    EndTextureMode();

}

void AttributeEditor::DrawUI()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Draw layout.
    ImGui::Begin(("Attributes - " + name).c_str(), &open);
    ImGui::SetWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();
    isFocused = ImGui::IsWindowFocused();
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Flags", ImGuiDataType_U8, &ed->tilesets[name].flags);
    ImGui::PushItemWidth(30);
    ImGui::InputScalar("Width", ImGuiDataType_U16, &ed->tilesets[name].oldWidth);
    ImGui::PushItemWidth(30);
    ImGui::SameLine();
    ImGui::InputScalar("Height", ImGuiDataType_U16, &ed->tilesets[name].oldHeight);
    if (ImGui::Button("Adjust Size"))
    {
        ed->tilesets[name].width = ed->tilesets[name].oldWidth;
        ed->tilesets[name].height = ed->tilesets[name].oldHeight;
        // TODO: ATTRIBUTE SHIFTING??? MAP FIXING???
        ed->ResizeAllTilesetViewers(name);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        ed->tilesets[name].Write(ed->rsc, name);
    }
    ImGui::RadioButton("Paint", &mode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Erase", &mode, 1);

    // Calculate tileset image size and draw it.
    float w = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();
    float h = ImGui::GetWindowHeight() - ImGui::GetCursorPosY();
    float texW = target.texture.width;
    float texH = target.texture.height;
    if (w / texW > h / texH)
    {
        float scale = h / texH;
        w = texW * scale;
        h = texH * scale;
    }
    else
    {
        float scale = w / texW;
        w = texW * scale;
        h = texH * scale;
    }
    imgPos = ImGui::GetCursorScreenPos();
    imgSizeX = w;
    imgSizeY = h;
    ImVec2 tmp = ImGui::GetCursorPos();
    RLImGuiImageSize(&finalTarget.texture, (int)h, (int)w);
    ImGui::SetCursorPos(tmp);
    ImGui::InvisibleButton("NoDrag", ImVec2((int)w, (int)h));
    ImGui::End();

}

void AttributeEditor::Update()
{

    // Safety.
    if (!open)
    {
        return;
    }
    else if (ed->tilesets.find(name) == ed->tilesets.end())
    {
        open = false;
        return;
    }

    // Get attribute selection.
    float scale = imgSizeX / target.texture.width;
    attrSelection.OneClickSelection(
        Tileset::EDITOR_TILE_SIZE * scale,
        MOUSE_LEFT_BUTTON,
        isFocused,
        0,
        0,
        Tileset::attrTex.width / Tileset::ATTR_TILE_SIZE,
        Tileset::attrTex.height / Tileset::ATTR_TILE_SIZE,
        imgPos.x + ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE * scale,
        imgPos.y,
        0,
        0,
        Tileset::attrTex.width * scale,
        Tileset::attrTex.height * scale
    );
    if (attrSelection.validSelection)
    {
        attrTile = Tileset::attrTex.width / Tileset::ATTR_TILE_SIZE * attrSelection.y + attrSelection.x;
    }

    // Get tile selection.
    tileSelection.OneClickSelection(
        Tileset::EDITOR_TILE_SIZE * scale,
        MOUSE_LEFT_BUTTON,
        isFocused,
        0,
        0,
        ed->tilesets[name].width,
        ed->tilesets[name].height,
        imgPos.x,
        imgPos.y,
        0,
        0,
        ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE * scale,
        ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE * scale
    );

    // Either paint or erase depending on the mode.
    if (tileSelection.validSelection)
    {
        int tNum = ed->tilesets[name].width * tileSelection.y + tileSelection.x;
        if (!mode)
        {
            ed->tilesets[name].tiles[tNum] = attrTile;
        }
        else
        {
            ed->tilesets[name].tiles[tNum] = 0;
        }
        tileSelection.ClearSelection();
    }

}

void AttributeEditor::Close()
{
    UnloadRenderTexture(target);
    UnloadRenderTexture(finalTarget);
}