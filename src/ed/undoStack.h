#pragma once

#include <deque>
#include "../types.h"

enum class UndoActionType {
    TilePlaced,
};

struct UndoAction {
    UndoActionType type;
    union {
        struct {
            u8 layer;
            u8 prevTile;
            u8 nextTile;
        } tilePlaced;
    } data;
};

struct Editor;

class UndoStack {
private:
    Editor* editor = nullptr;
    std::deque<UndoAction> stack;
public:
};