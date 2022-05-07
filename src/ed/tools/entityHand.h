#pragma once

#include "raylib.h"

struct Editor;

void EntityHandToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2);
void EntityHandToolUpdate(Editor* ed);
void EntityHandToolClose(Editor* ed);