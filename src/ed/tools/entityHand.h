#pragma once

#include "raylib.h"

struct EditorNew;

void EntityHandToolActivate(EditorNew* ed, Vector2 pos1, Vector2 pos2);
void EntityHandToolUpdate(EditorNew* ed);
void EntityHandToolClose(EditorNew* ed);