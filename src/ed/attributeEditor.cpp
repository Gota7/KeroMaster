#include "attributeEditor.h"
#include "../rlImGui/rlImGui.h"

using namespace std;

AttributeEditor::AttributeEditor(Editor* ed, string name)
{
    open = true;
    this->ed = ed;
    this->name = name;
    if (ed->tilesets.find(name) == ed->tilesets.end())
    {
        open = false;
    }
    else if (ed->tilesets[name].tiles == NULL)
    {
        ed->tilesets[name].tiles = new u8[ed->tilesets[name].width * ed->tilesets[name].height];
    }
    target = LoadRenderTexture(ed->tilesets[name].width * 8 * 2 + (float)Tileset::attrTex.width, max((float)ed->tilesets[name].height * 8 * 2, (float)Tileset::attrTex.height));
}

void AttributeEditor::Draw()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Draw data.
    BeginTextureMode(target);
    ClearBackground(BLACK);
    DrawTexturePro(ed->tilesets[name].tex, { 0, 0, (float)ed->tilesets[name].width * 8, -(float)ed->tilesets[name].height * 8 }, { 0, 0, (float)ed->tilesets[name].width * 8 * 2, (float)ed->tilesets[name].height * 8 * 2 }, { 0, 0 }, 0, WHITE);
    DrawTextureRec(Tileset::attrTex, { 0, 0, (float)Tileset::attrTex.width, -(float)Tileset::attrTex.height }, { (float)ed->tilesets[name].width * 8 * 2, 0 }, WHITE);
    DrawLineEx( { (float)ed->tilesets[name].width * 8 * 2, 0 }, { (float)ed->tilesets[name].width * 8 * 2, (float)ed->tilesets[name].height * 8 * 2 }, 3, RED);
    u16 x = attrTile % 16;
    u16 y = 16 - attrTile / 16 - 1;
    DrawRectangleLinesEx( { (float)x * 8 * 2 + ed->tilesets[name].width * 8 * 2, (float)y * 8 * 2, (float)8 * 2, (float)8 * 2 }, 1, ColorAlpha(YELLOW, .5f));
    for (int i = 0; i < ed->tilesets[name].width; i++)
    {
        for (int j = 0; j < ed->tilesets[name].height; j++)
        {
            u8 attr = ed->tilesets[name].GetTilesetAttr(i, j);
            DrawTextureRec(Tileset::attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, -16 }, { (float)i * 16, (float)(ed->tilesets[name].height - j - 1) * 16 }, WHITE);
        }
    }
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
    RLImGuiImageSize(&target.texture, (int)h, (int)w);
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

    // Get action.
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    if (ed->focus.mouseInWindow && \
        mouseX > imgPos.x && \
        mouseX < imgPos.x + imgSizeX && \
        mouseY > imgPos.y && \
        mouseY < imgPos.y + imgSizeY && \
        isFocused && \
        IsMouseButtonDown(MOUSE_LEFT_BUTTON)
    )
    {
        int tX = (mouseX - imgPos.x) / imgSizeX * (ed->tilesets[name].width + 16);
        int tY = (mouseY - imgPos.y) / imgSizeY * max(ed->tilesets[name].height, (u16)16);
        bool left = true;
        if (tX >= ed->tilesets[name].width)
        {
            left = false;
            tX -= ed->tilesets[name].width;
        }
        if (left)
        {
            if (tX >= 0 && tX < ed->tilesets[name].width && tY >= 0 && tY < ed->tilesets[name].height)
            {
                int tNum = ed->tilesets[name].width * tY + tX;
                if (mode == 0 && attrTile != -1)
                {
                    ed->tilesets[name].tiles[tNum] = attrTile;
                }
                else if (mode == 1)
                {
                    ed->tilesets[name].tiles[tNum] = 0;
                }
            }
        }
        else
        {
            if (tY < 16 && tY >= 0 && tX >= 0 && tX < 16)
            {
                attrTile = tY * 16 + tX;
            }
        }
    }

}

void AttributeEditor::Close()
{
    UnloadRenderTexture(target);
}