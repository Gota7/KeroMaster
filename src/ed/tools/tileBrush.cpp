#include "tileBrush.h"
#include "../editor.h"

void TileBrushToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2)
{
    if (ed->tilesToPaint.validSelection)
    {
        u16 startX = (u16)pos1.x;
        u16 startY = (u16)pos1.y;
        PxMap& layer = ed->map.maps[ed->tilesToPaint.layer];
        Tileset& t = ed->tilesets[ed->map.tilesets[ed->tilesToPaint.layer].dat];
        for (int x = 0; x < ed->tilesToPaint.width; x++)
        {
            for (int y = 0; y < ed->tilesToPaint.height; y++)
            {
                u16 xPos = startX + x;
                u16 yPos = startY + y;
                if (xPos >= 0 && yPos >= 0 && xPos < layer.width && yPos < layer.height)
                {
                    u8 oldTile = layer.GetTile(xPos, yPos);
                    u8 newTile = ed->tilesToPaint.x + x + (ed->tilesToPaint.y + y) * t.width;
                    layer.SetTile(xPos, yPos, newTile);
                    if (oldTile != newTile)
                    {
                        ed->undoStack.PushTilePlaced(ed, ed->tilesToPaint.layer, oldTile, newTile, xPos, yPos);
                        if (!(x == 0 && y == 0)) ed->undoStack.SetLastChained();
                    }
                }
            }
        }
    }
}

void TileBrushToolUpdate(Editor* ed)
{
    TileBrushToolActivate(ed, { (float)ed->GetTileX(), (float)ed->GetTileY() }, { 0, 0 });
}