#include "tilesetEditor.h"

#include "editorNew.h"
#include "../rlImGui/rlImGui.h"
#include "../px/tileset.h"

TilesetEditor::TilesetEditor(EditorNew* ed, std::string name, float tileSize)
{
    open = true;
    this->ed = ed;
    this->name = name;
    if (ed->tilesets.find(name) == ed->tilesets.end())
    {
        open = false; // Close immediately if tileset not found.
    }
    this->tileSize = tileSize;
    target = LoadRenderTexture(ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE);
    finalTarget = LoadRenderTexture(ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE);
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
    DrawTexturePro(
        ed->tilesets[name].tex,
        {
            0,
            0,
            (float)ed->tilesets[name].width * tileSize,
            (float)ed->tilesets[name].height * tileSize
        },
        {
            0,
            0,
            (float)ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE,
            (float)ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE
        },
        { 0, 0 },
        0,
        WHITE
    );

    // Draw attributes on top.
    if (viewAttr && ed->tilesets[name].tiles != NULL)
    {
        for (u16 x = 0; x < ed->tilesets[name].width; x++)
        {
            for (u16 y = 0; y < ed->tilesets[name].height; y++)
            {
                u8 attr = ed->tilesets[name].GetTilesetAttr(x, y);
                if (attr != 0)
                {
                    Tileset::DrawTileAttr(attr, x, y, Tileset::EDITOR_TILE_SIZE);
                }
            }
        }
    }

    // Draw the currently selected tile.
    if (selection.validSelection)
    {
        DrawRectangleLinesEx(
            {
                (float)selection.x * Tileset::EDITOR_TILE_SIZE,
                (float)selection.y * Tileset::EDITOR_TILE_SIZE,
                (float)selection.width * Tileset::EDITOR_TILE_SIZE,
                (float)selection.height * Tileset::EDITOR_TILE_SIZE
            },
            2,
            WHITE);
    }
    EndTextureMode();

    // Hack - Y flip the target by drawing it on another final target that is the same size.
    BeginTextureMode(finalTarget);
    DrawTexture(target.texture, 0, 0, WHITE);
    EndTextureMode();

}

void TilesetEditor::DrawUI()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Determine which layers are allowed and selected.
    ImGui::Begin(("Tileset - " + name).c_str(), &open);
    ImGui::SetWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();
    focused = ImGui::IsWindowHovered();
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

    // Attribute editing.
    ImGui::Checkbox("View Attributes", &viewAttr);
    ImGui::SameLine();
    if (ImGui::Button("Edit Attributes"))
    {
        ed->OpenAttr(name, 8.0f); // TODO: GIVE THIS PROPER TILE SIZE!!!
    }

    // Scale the tileset image properly.
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

    // Set variables, draw the tileset image, and use an invisible button to prevent moving the window when selecting tiles.
    imgPos = ImGui::GetCursorScreenPos();
    imgSizeX = w;
    imgSizeY = h;
    ImVec2 tmp = ImGui::GetCursorPos();
    RLImGuiImageSize(&finalTarget.texture, (int)h, (int)w);
    ImGui::SetCursorPos(tmp);
    ImGui::InvisibleButton("NoDrag", ImVec2((int)w, (int)h));
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

    // Update selection.
    selection.Update(
        imgSizeX / ed->tilesets[name].width,
        MOUSE_LEFT_BUTTON,
        focused,
        0,
        0,
        ed->tilesets[name].width,
        ed->tilesets[name].height,
        imgPos.x,
        imgPos.y,
        0,
        0,
        imgSizeX,
        imgSizeY
    );
    selection.layer = currLayer;

    // Update editor's selection.
    if (selection.validSelection && !selection.isSelecting && focused)
    {
        selectedTile = selection.y * ed->tilesets[name].width + selection.x;
        ed->RemoveAllOtherTilesetViewerSelections(this);
        ed->tilesToPaint = selection;
    }
    else if (!selection.validSelection || selection.isSelecting)
    {
        selectedTile = -1;
    }
    if (selection.isSelecting)
    {
        ed->RemoveAllOtherTilesetViewerSelections(this);
        ed->tilesToPaint = selection;
    }

}

void TilesetEditor::Close()
{
    open = false;
    UnloadRenderTexture(target);
    UnloadRenderTexture(finalTarget);
}