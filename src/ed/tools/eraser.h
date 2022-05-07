#pragma once

#include "raylib.h"

struct EditorNew;

void EraserToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2);
void EraserToolUpdate(EditorNew* ed);