#pragma once

#include "raylib.h"

struct EditorNew;

void ZoomToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2);
void ZoomToolUpdate(EditorNew* ed);