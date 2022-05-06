#include "hand.h"
#include "../editorNew.h"

void HandToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2)
{
    ed->MoveCamX(pos2.x - pos1.x);
    ed->MoveCamY(pos2.y - pos1.y);
}

void HandToolUpdate(EditorNew* ed)
{
    HandToolActivate(ed, { ed->oldMouseX, ed->oldMouseY }, { ed->mouseX, ed->mouseY });
}