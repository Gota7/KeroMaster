#include "palette.h"

#include "editor.h"
#include "../rlImGui/rlImGui.h"

Palette::Palette(Editor* ed)
{
    this->ed = ed;
}

void Palette::Draw()
{

    // Safety.
    if (name == "" || ed->currTool != ToolItem::TileBrush || !ed->settings.usePalette)
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

void Palette::DrawUI()
{

    // Safety.
    if (name == "" || ed->currTool != ToolItem::TileBrush || !ed->settings.usePalette)
    {
        return;
    }

    // Determine which layers are allowed and selected.
    ImGui::Begin("Palette");
    ImGui::SetWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();
    focused = ImGui::IsWindowHovered();

    // Attribute editing.
    ImGui::Checkbox("View Attributes", &viewAttr);

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

void Palette::Update()
{

    // Safety.
    if (name == "" || ed->currTool != ToolItem::TileBrush || !ed->settings.usePalette)
    {
        return;
    }
    else if (ed->tilesets.find(name) == ed->tilesets.end())
    {
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
    selection.layer = ed->currentLayer;

    // Update editor's selection.
    if (selection.validSelection && !selection.isSelecting && focused)
    {
        selectedTile = selection.y * ed->tilesets[name].width + selection.x;
        ed->RemoveAllOtherTilesetViewerSelections(nullptr);
        ed->tilesToPaint = selection;
    }
    else if (!selection.validSelection || selection.isSelecting)
    {
        selectedTile = -1;
    }
    if (selection.isSelecting)
    {
        ed->RemoveAllOtherTilesetViewerSelections(nullptr);
        ed->tilesToPaint = selection;
    }

}

void Palette::ChangeTileset(std::string name)
{

    // Unload previous.
    if (this->name == "")
    {
        UnloadRenderTexture(target);
        UnloadRenderTexture(finalTarget);
    }

    // Load new data.
    this->name = name;
    if (name != "")
    {
        if (ed->tilesets.find(name) == ed->tilesets.end())
        {
            name = ""; // Close immediately if tileset not found.
            return;
        }
        tileSize = 8.0f; // TODO: GET TRUE TILESIZE!!!
        target = LoadRenderTexture(ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE);
        finalTarget = LoadRenderTexture(ed->tilesets[name].width * Tileset::EDITOR_TILE_SIZE, ed->tilesets[name].height * Tileset::EDITOR_TILE_SIZE);
    }

}