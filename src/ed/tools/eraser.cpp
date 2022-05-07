#include "eraser.h"
#include "../editorNew.h"

void EraserToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2)
{
    PxMap& layer = ed->map.maps[ed->currentLayer];
    layer.SetTile((u16)pos1.x, (u16)pos1.y, 0);
}

void EraserToolUpdate(EditorNew* ed)
{
    EraserToolActivate(ed, { (float)ed->GetTileX(), (float)ed->GetTileY() }, { 0, 0 });
}