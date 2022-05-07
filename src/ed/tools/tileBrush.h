#pragma once

#include "raylib.h"

struct EditorNew;

void TileBrushToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2);
void TileBrushToolUpdate(EditorNew* ed);