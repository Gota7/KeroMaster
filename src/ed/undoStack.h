#pragma once

#include <deque>
#include "../types.h"
#include "editor.h"

struct Editor;

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
    UndoActionType type;
    struct {
        struct {
            u8 layer;
            u8 prevTile;
            u8 nextTile;
        } tilePlaced;
        struct {
            u16 index;
            u8 type;
            u16 lastX;
            u16 lastY;
            u16 newX;
            u16 newY;
        } entityExPosition;
        struct {
            u16 index;
            Entity oldEntity;
        } entityCapture;
    } data;
};

struct UndoStack {
private:
    std::deque<UndoAction> undoStack;
    std::deque<UndoAction> redoStack;
    int maxSize = 20;
public:
    void SetMaxLen(int maxSize);
    void Reset();
    void PushTilePlace(Editor* ed);
    void PushTileErase(Editor* ed);
    void PushLevelPropertyEdited(Editor* ed);
    void PushEntityPropertyEdited(Editor* ed);
    void PushEntityMoved(Editor* ed, int entityIndex, u16 xPos, u16 yPos, u16 newXPos, u16 newYPos);
    void PushEntityPlaced(Editor* ed, int entityIndex, u8 type, u16 xPos, u16 yPos);
    void PushEntityDeleted(Editor* ed, int entityIndex, Entity e);
    void Undo(Editor* ed);
    void Redo(Editor* ed);
    bool CanUndo();
    bool CanRedo();
};