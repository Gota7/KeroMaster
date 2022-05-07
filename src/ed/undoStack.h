#pragma once

#include "entityDisplay.h"
#include "../types.h"
#include <deque>

struct EditorNew;

enum class UndoActionType {
    TilePlaced,
    TileErased,
    LevelPropertyEdited,
    EntityPropertyEdited,
    EntityMoved,
    EntityPlaced,
    EntityDeleted
};

struct UndoAction {
    UndoActionType type; // Type of the undo action.
    bool chained = false; // If this action is not the only one and should also undo the previous action.
    struct {
        struct {
            u16 xPos;
            u16 yPos;
            u8 layer;
            u8 prevTile;
            u8 nextTile;
        } tilePlaced; // A tile has been placed.
        struct {
            u8 type;
            u16 index;
            u16 lastX;
            u16 lastY;
            u16 newX;
            u16 newY;
        } entityExPosition; // An entity has moved.
        struct {
            u16 index;
            Entity oldEntity;
        } entityCapture; // An entity has been removed.
    } data;
};

struct UndoStack {
private:
    std::deque<UndoAction> undoStack;
    std::deque<UndoAction> redoStack;
    int maxSize = 20;
public:
    void SetMaxLen(int maxSize);
    int MaxLen();
    void Reset();
    void PushUndoAction(UndoAction& a);
    void PushTilePlaced(EditorNew* ed, u8 layer, u8 oldTile, u8 newTile, u16 xPos, u16 yPos);
    void PushTileErased(EditorNew* ed, u8 layer, u8 oldTile, u16 xPos, u16 yPos);
    void PushLevelPropertyEdited(EditorNew* ed);
    void PushEntityPropertyEdited(EditorNew* ed);
    void PushEntityMoved(EditorNew* ed, int entityIndex, u16 xPos, u16 yPos, u16 newXPos, u16 newYPos);
    void PushEntityPlaced(EditorNew* ed, int entityIndex, u8 type, u16 xPos, u16 yPos);
    void PushEntityDeleted(EditorNew* ed, int entityIndex, Entity e);
    void SetLastChained();
    void Undo(EditorNew* ed);
    void Redo(EditorNew* ed);
    bool CanUndo();
    bool CanRedo();
};