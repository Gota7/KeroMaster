#include "eraser.h"
#include "../editorNew.h"

void EraserToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2)
{
    PxMap& layer = ed->map.maps[ed->currentLayer];
    u8 oldTile = layer.GetTile((u16)pos1.x, (u16)pos1.y);
    layer.SetTile((u16)pos1.x, (u16)pos1.y, 0);
    if (oldTile != 0) ed->undoStack.PushTileErased(ed, ed->currentLayer, oldTile, (u16)pos1.x, (u16)pos1.y);
}

void EraserToolUpdate(EditorNew* ed)
{
    EraserToolActivate(ed, { (float)ed->GetTileX(), (float)ed->GetTileY() }, { 0, 0 });
}