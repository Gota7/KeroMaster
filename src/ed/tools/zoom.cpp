#include "zoom.h"
#include "../editor.h"

void ZoomToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2)
{
    ed->Zoom(pos1, pos2.x, pos2.y != 0);
}

void ZoomToolUpdate(Editor* ed)
{
    float isNeg = (IsKeyDown(KEY_LEFT_SHIFT) | IsKeyDown(KEY_RIGHT_SHIFT)) ? -1 : 1;
    ZoomToolActivate(ed, { ed->mouseX, ed->mouseY }, { ed->GetZoomSpeed() * isNeg * 2, 1 });
    ed->tools.toolActive[(int)ToolItem::Zoom] = false;
}