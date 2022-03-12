#include "undoStack.h"
#include "editor.h"

void UndoStack::SetMaxLen(int maxSize)
{
    this->maxSize = maxSize;
}

void UndoStack::Reset()
{
    undoStack.clear();
    redoStack.clear();
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
    undoStack.push_front(a);
    redoStack.clear();
    if (undoStack.size() > maxSize)
    {
        undoStack.pop_back();
    }
}

void UndoStack::PushEntityPlaced(Editor* ed, int entityIndex, u8 type, u16 xPos, u16 yPos)
{
    UndoAction a;
    a.type = UndoActionType::EntityPlaced;
    a.data.entityExPosition.index = entityIndex;
    a.data.entityExPosition.type = type;
    a.data.entityExPosition.lastX = xPos;
    a.data.entityExPosition.lastY = yPos;
    undoStack.push_front(a);
    redoStack.clear();
    if (undoStack.size() > maxSize)
    {
        undoStack.pop_back();
    }
}

void UndoStack::PushEntityDeleted(Editor* ed, int entityIndex, Entity e)
{
    UndoAction a;
    a.type = UndoActionType::EntityDeleted;
    a.data.entityCapture.index = entityIndex;
    a.data.entityCapture.oldEntity = e;
    undoStack.push_front(a);
    redoStack.clear();
    if (undoStack.size() > maxSize)
    {
        undoStack.pop_back();
    }
}

void UndoStack::Undo(Editor* ed)
{
    if (!CanUndo()) return;
    UndoAction a = undoStack.front();
    undoStack.pop_front();
    switch (a.type)
    {
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
}

void UndoStack::Redo(Editor* ed)
{
    if (!CanRedo()) return;
    UndoAction a = redoStack.front();
    redoStack.pop_front();
    switch (a.type)
    {
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
                e->parametersByte[0] = 0;
                e->parametersByte[1] = 0;
                e->parametersStr[0].dat = "";
                e->unk = 0;
                break;
            }
        case UndoActionType::EntityDeleted:
            ed->map.entities.erase(ed->map.entities.begin() + a.data.entityCapture.index);
            break;
        default:
            break;
    }
    undoStack.push_front(a);
}

bool UndoStack::CanUndo()
{
    return undoStack.size() > 0;
}

bool UndoStack::CanRedo()
{
    return redoStack.size() > 0;
}