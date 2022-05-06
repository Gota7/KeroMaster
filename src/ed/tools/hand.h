#pragma once

#include "raylib.h"

struct EditorNew;

void HandToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2);
void HandToolUpdate(EditorNew* ed);