#include "tilesetEditor.h"
#include "../rlImGui/rlImGui.h"

TilesetEditor::TilesetEditor(Editor* ed, string name)
{
    open = true;
    this->ed = ed;
    this->name = name;
    if (ed->tilesets.find(name) == ed->tilesets.end())
    {
        open = false;
    }
    target = LoadRenderTexture(ed->tilesets[name].width * 8 * 2, ed->tilesets[name].height * 8 * 2);
}

void TilesetEditor::Draw()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Draw target.
    BeginTextureMode(target);
    ClearBackground(BLACK);
    DrawTexturePro(ed->tilesets[name].tex, { 0, 0, (float)ed->tilesets[name].width * 8, -(float)ed->tilesets[name].height * 8 }, { 0, 0, (float)ed->tilesets[name].width * 8 * 2, (float)ed->tilesets[name].height * 8 * 2}, { 0, 0 }, 0, WHITE);
    if (viewAttr && ed->tilesets[name].tiles != NULL)
    {
        for (u16 x = 0; x < ed->tilesets[name].width; x++)
        {
            for (u16 y = 0; y < ed->tilesets[name].height; y++)
            {
                u8 attr = ed->tilesets[name].GetTilesetAttr(x, y);
                if (attr != 0)
                {
                    DrawTextureRec(Tileset::attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, -16 }, { (float)x * 16, (float)(ed->tilesets[name].height - y - 1) * 16 }, WHITE);
                }
            }
        }
    }
    if (currTile >= 0 && currTile < ed->tilesets[name].width * ed->tilesets[name].height)
    {
        u16 x = currTile % ed->tilesets[name].width;
        u16 y = ed->tilesets[name].height - currTile / ed->tilesets[name].width - 1;
        DrawRectangleLinesEx( { (float)x * 8 * 2, (float)(y - selectionHeight + 1) * 8 * 2, (float)8 * 2 * selectionWidth, (float)8 * 2 * selectionHeight }, 1, ColorAlpha(YELLOW, .5f));
    }
    EndTextureMode();

}

void TilesetEditor::DrawUI()
{
    
    // Safety.
    if (!open)
    {
        return;
    }

    // Interface.
    ImGui::Begin(("Tileset - " + name).c_str(), &open);
    ed->focus.ObserveFocus();
    if (allowLayer0)
    {
        ImGui::RadioButton("Foreground", &currLayer, 0);
        if (allowLayer1 || allowLayer2) ImGui::SameLine();
    }
    if (allowLayer1)
    {
        ImGui::RadioButton("Middleground", &currLayer, 1);
        if (allowLayer2) ImGui::SameLine();
    }
    if (allowLayer2)
    {
        ImGui::RadioButton("Background", &currLayer, 2);
    }
    ImGui::Checkbox("View Attributes", &viewAttr);
    ImGui::SameLine();
    if (ImGui::Button("Edit Attributes"))
    {
        ed->OpenAttrEditor(name);
    }
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
    RLImGuiImageSize(&target.texture, (int)h, (int)w);
    ImGui::End();
    
}

void TilesetEditor::Update()
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

    // Check.
    if (currTile >= ed->tilesets[name].width * ed->tilesets[name].height)
    {
        currTile = -1;
    }

    // Tile clicked.
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    if (ed->focus.mouseInWindow && \
        mouseX > imgPos.x && \
        mouseX < imgPos.x + imgSizeX && \
        mouseY > imgPos.y && \
        mouseY < imgPos.y + imgSizeY && \
        IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
    )
    {
        startMouseX = mouseX;
        startMouseY = mouseY;
        CalcTiles();
        selectingTiles = true;
    }
    else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && selectingTiles)
    {
        selectingTiles = false;
        CalcTiles();
    }
    else if (selectingTiles)
    {
        CalcTiles();
    }

}

void TilesetEditor::CalcTiles()
{
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    int startTileX = (startMouseX - imgPos.x) / imgSizeX * ed->tilesets[name].width;
    int startTileY = (startMouseY - imgPos.y) / imgSizeY * ed->tilesets[name].height;
    int endTileX = (mouseX - imgPos.x) / imgSizeX * ed->tilesets[name].width;
    int endTileY = (mouseY - imgPos.y) / imgSizeY * ed->tilesets[name].height;
    if (endTileX >= ed->tilesets[name].width)
    {
        endTileX = ed->tilesets[name].width - 1;
    }
    else if (endTileX < 0)
    {
        endTileX = 0;
    }
    if (endTileY >= ed->tilesets[name].height)
    {
        endTileY = ed->tilesets[name].height - 1;
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
    currTile = currTileX + currTileY * ed->tilesets[name].width;
    if (currTile != -1)
    {
        ed->RemoveAllOtherTilesetViewerSelections(this);
        ed->editingTileset = this;
    }
}

void TilesetEditor::Close()
{
    if (ed->editingTileset == this)
    {
        ed->editingTileset = nullptr;
    }
    open = false;
    UnloadRenderTexture(target);
}