#pragma once

#include "raylib.h"

struct Editor;

void HandToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2);
void HandToolUpdate(Editor* ed);