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
        u16 y = ed->tilesets[name].height - currTile / ed->tilesets[name].height - 1;
        DrawRectangleLinesEx( { (float)x * 8 * 2, (float)y * 8 * 2, 8 * 2, 8 * 2 }, 1, ColorAlpha(YELLOW, .5f));
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
    const int itemWidth = 150;
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Flags", ImGuiDataType_U8, &ed->tilesets[name].flags);
    ImGui::PushItemWidth(30);
    ImGui::InputScalar("Width", ImGuiDataType_U16, &ed->tilesets[name].oldWidth);
    ImGui::PushItemWidth(30);
    ImGui::SameLine();
    ImGui::InputScalar("Height", ImGuiDataType_U16, &ed->tilesets[name].oldHeight);
    ImGui::SameLine();
    if (ImGui::Button("Adjust Size"))
    {

    }
    ImGui::Checkbox("View Attributes", &viewAttr);
    ImGui::SameLine();
    if (ImGui::Button("Edit Attributes"))
    {

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
    imgSize = w;
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

    // Tile clicked.
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    if (ed->focus.mouseInWindow && \
        mouseX > imgPos.x && \
        mouseX < imgPos.x + imgSize && \
        mouseY > imgPos.y && \
        mouseY < imgPos.y + imgSize && \
        IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
    )
    {
        int tileX = (mouseX - imgPos.x) / imgSize * ed->tilesets[name].width;
        int tileY = (mouseY - imgPos.y) / imgSize * ed->tilesets[name].height;
        currTile = tileY * ed->tilesets[name].width + tileX;
    }
}

void TilesetEditor::Close()
{
    open = false;
    UnloadRenderTexture(target);
}