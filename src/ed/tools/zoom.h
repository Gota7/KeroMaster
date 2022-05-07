#pragma once

#include "raylib.h"

struct Editor;

void ZoomToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2);
void ZoomToolUpdate(Editor* ed);