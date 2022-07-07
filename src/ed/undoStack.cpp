#include "undoStack.h"
#include "editor.h"

void UndoStack::SetMaxLen(int maxSize)
{
    this->maxSize = maxSize;
    while (redoStack.size() > maxSize)
    {
        redoStack.pop_back();
    }
    while (undoStack.size() > maxSize)
    {
        undoStack.pop_back();
    }
}

int UndoStack::MaxLen()
{
    return maxSize;
}

void UndoStack::Reset()
{
    undoStack.clear();
    redoStack.clear();
}

void UndoStack::PushUndoAction(UndoAction& a)
{
    undoStack.push_front(a);
    redoStack.clear();
    if (undoStack.size() > maxSize)
    {
        undoStack.pop_back();
    }
}

void UndoStack::PushTilePlaced(Editor* ed, u8 layer, u8 oldTile, u8 newTile, u16 xPos, u16 yPos)
{
    UndoAction a;
    a.type = UndoActionType::TilePlaced;
    a.data.tilePlaced.layer = layer;
    a.data.tilePlaced.prevTile = oldTile;
    a.data.tilePlaced.nextTile = newTile;
    a.data.tilePlaced.xPos = xPos;
    a.data.tilePlaced.yPos = yPos;
    PushUndoAction(a);
}

void UndoStack::PushTileErased(Editor* ed, u8 layer, u8 oldTile, u16 xPos, u16 yPos)
{
    PushTilePlaced(ed, layer, oldTile, 0, xPos, yPos);
}

void UndoStack::PushEntityMoved(Editor* ed, int entityIndex, u16 xPos, u16 yPos, u16 newXPos, u16 newYPos)
{
    UndoAction a;
    a.type = UndoActionType::EntityMoved;
    a.data.entityExPosition.index = entityIndex;
    a.data.entityExPosition.lastX = xPos;
    a.data.entityExPosition.lastY = yPos;
    a.data.entityExPosition.newX = newXPos;
    a.data.entityExPosition.newY = newYPos;
    PushUndoAction(a);
}

void UndoStack::PushEntityPlaced(Editor* ed, int entityIndex, u8 type, u16 xPos, u16 yPos)
{
    UndoAction a;
    a.type = UndoActionType::EntityPlaced;
    a.data.entityExPosition.index = entityIndex;
    a.data.entityExPosition.type = type;
    a.data.entityExPosition.lastX = xPos;
    a.data.entityExPosition.lastY = yPos;
    PushUndoAction(a);
}

void UndoStack::PushEntityDeleted(Editor* ed, int entityIndex, Entity e)
{
    UndoAction a;
    a.type = UndoActionType::EntityDeleted;
    a.data.entityCapture.index = entityIndex;
    a.data.entityCapture.oldEntity = e;
    PushUndoAction(a);
}

void UndoStack::SetLastChained()
{
    if (CanUndo())
    {
        undoStack.front().chained = true;
    }
}

void UndoStack::Undo(Editor* ed)
{
    if (!CanUndo()) return;
    UndoAction a = undoStack.front();
    undoStack.pop_front();
    switch (a.type)
    {
        case UndoActionType::TilePlaced:
        case UndoActionType::TileErased:
            ed->map.maps[a.data.tilePlaced.layer].SetTile(a.data.tilePlaced.xPos, a.data.tilePlaced.yPos, a.data.tilePlaced.prevTile);
            break;
        case UndoActionType::EntityMoved:
            ed->map.entities[a.data.entityExPosition.index].xPos = a.data.entityExPosition.lastX;
            ed->map.entities[a.data.entityExPosition.index].yPos = a.data.entityExPosition.lastY;
            break;
        case UndoActionType::EntityPlaced:
            ed->map.entities.erase(ed->map.entities.begin() + a.data.entityExPosition.index);
            break;
        case UndoActionType::EntityDeleted:
            ed->map.entities.insert(ed->map.entities.begin() + a.data.entityCapture.index, a.data.entityCapture.oldEntity);
            break;
        default:
            break;
    }
    redoStack.push_front(a);
    if (a.chained) Undo(ed); // Undo until no longer chained.
}

void UndoStack::Redo(Editor* ed)
{
    if (!CanRedo()) return;
    UndoAction a = redoStack.front();
    redoStack.pop_front();
    switch (a.type)
    {
        case UndoActionType::TilePlaced:
        case UndoActionType::TileErased:
            ed->map.maps[a.data.tilePlaced.layer].SetTile(a.data.tilePlaced.xPos, a.data.tilePlaced.yPos, a.data.tilePlaced.nextTile);
            break;
        case UndoActionType::EntityMoved:
            ed->map.entities[a.data.entityExPosition.index].xPos = a.data.entityExPosition.newX;
            ed->map.entities[a.data.entityExPosition.index].yPos = a.data.entityExPosition.newY;
            break;
        case UndoActionType::EntityPlaced:
            {
                ed->map.entities.insert(ed->map.entities.begin() + a.data.entityExPosition.index, Entity());
                Entity* e = &ed->map.entities[a.data.entityExPosition.index];
                e->id = a.data.entityExPosition.type;
                e->xPos = a.data.entityExPosition.lastX;
                e->yPos = a.data.entityExPosition.lastY;
                e->beingEdited = false;
                e->flags = 1;
                e->flag = 0;
                e->data.dat = "";
                e->variant = 0;
                break;
            }
        case UndoActionType::EntityDeleted:
            ed->map.entities.erase(ed->map.entities.begin() + a.data.entityCapture.index);
            break;
        default:
            break;
    }
    undoStack.push_front(a);
    if (CanRedo() && redoStack.front().chained) Redo(ed); // Do chained redos, as ones next to the current one are chained.
}

bool UndoStack::CanUndo()
{
    return undoStack.size() > 0;
}

bool UndoStack::CanRedo()
{
    return redoStack.size() > 0;
}