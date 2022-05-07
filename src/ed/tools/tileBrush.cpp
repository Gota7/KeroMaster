#include "tileBrush.h"
#include "../editorNew.h"

void TileBrushToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2)
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
                    layer.SetTile(xPos, yPos, ed->tilesToPaint.x + x + (ed->tilesToPaint.y + y) * t.width);
                }
            }
        }
    }
}

void TileBrushToolUpdate(EditorNew* ed)
{
    TileBrushToolActivate(ed, { (float)ed->GetTileX(), (float)ed->GetTileY() }, { 0, 0 });
}