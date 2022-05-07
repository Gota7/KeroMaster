#pragma once

#include "raylib.h"

struct Editor;

void TileBrushToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2);
void TileBrushToolUpdate(Editor* ed);