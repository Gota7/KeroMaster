#include "entityHand.h"
#include "../editorNew.h"

u16 bakMoveX;
u16 bakMoveY;
u16 newMoveX;
u16 newMoveY;
u16 movingIndex;
u8 movingType;

void EntityHandToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2)
{

    // Get coordinates.
    int tileX = ed->GetTileX();
    int tileY = ed->GetTileY();

    // Place an entity.
    if (ed->isPlacingEntity)
    {
        if (tileX < 0) { return; }
        if (tileY < 0) { return; }
        if (tileX > 0xFFFF) { return; }
        if (tileY > 0xFFFF) { return; }
        Entity e = Entity();
        e.beingEdited = false;
        e.flags = 1;
        e.id = ed->placeEntityId;
        e.parametersByte[0] = 0;
        e.parametersByte[1] = 0;
        e.parametersStr[0].dat = "";
        e.unk = 0;
        e.xPos = tileX;
        e.yPos = tileY;
        ed->map.entities.push_back(e);
        if (IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT))
        {
            ed->isPlacingEntity = false;
        }
        ed->tools.toolActive[(int)ToolItem::EntityHand] = false;
        ed->undoStack.PushEntityPlaced(ed, ed->map.entities.size() - 1, e.id, e.xPos, e.yPos);
    }

    // Move the entity.
    else if (ed->doingEntityMove)
    {
        if (tileX < 0) { tileX = 0; }
        if (tileY < 0) { tileY = 0; }
        if (tileX > 0xFFFF) { tileX = 0xFFFF; }
        if (tileY > 0xFFFF) { tileY = 0xFFFF; }
        ed->entityEditor.editingEntity->xPos = tileX;
        ed->entityEditor.editingEntity->yPos = tileY;
        newMoveX = tileX;
        newMoveY = tileY;
    }

    // Select an entity.
    else
    {
        if (ed->entityEditor.editingEntity != nullptr) ed->entityEditor.editingEntity->beingEdited = false;
        if (tileX < 0) { tileX = 0; }
        if (tileY < 0) { tileY = 0; }
        if (tileX > 0xFFFF) { tileX = 0xFFFF; }
        if (tileY > 0xFFFF) { tileY = 0xFFFF; }
        u16 ind = 0;
        for (auto& e : ed->map.entities)
        {
            if (e.xPos == tileX && e.yPos == tileY)
            {
                ed->entityEditor.editingEntity = &e;
                ed->entityEditor.editingEntity->beingEdited = true;
                bakMoveX = ed->entityEditor.editingEntity->xPos;
                bakMoveY = ed->entityEditor.editingEntity->yPos;
                movingIndex = ind;
                movingType = ed->entityEditor.editingEntity->id;
                ed->doingEntityMove = true;
                return;
            }
            ind++;
        }
        ed->entityEditor.editingEntity = nullptr;
        ed->tools.toolActive[(int)ToolItem::EntityHand] = false;
    }

}

void EntityHandToolUpdate(EditorNew* ed)
{
    EntityHandToolActivate(ed, { (float)ed->GetTileX(), (float)ed->GetTileY() }, { 0, 0 });
}

void EntityHandToolClose(EditorNew* ed)
{
    ed->doingEntityMove = false;
    if (newMoveX != bakMoveX || newMoveY != bakMoveY)
    {
        ed->undoStack.PushEntityMoved(ed, movingIndex, bakMoveX, bakMoveY, newMoveX, newMoveY);
    }
}