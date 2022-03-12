#include "hand.h"
#include "../editor.h"

void HandTool::Init(Editor* ed)
{
    oldMouseX = GetMouseX();
    oldMouseY = GetMouseY();
}

void HandTool::Activate(Editor* ed, Vector2 pos1, Vector2 pos2)
{
    ed->MoveCamX(pos2.x - pos1.x);
    ed->MoveCamY(pos2.y - pos1.y);
}

void HandTool::Update(Editor* ed)
{
    if (toolActive)
    {
        Activate(ed, { (float)oldMouseX, (float)oldMouseY }, { (float)GetMouseX(), (float)GetMouseY() });
    }
}