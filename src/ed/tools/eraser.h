#pragma once

#include "raylib.h"

struct Editor;

void EraserToolActivate(Editor* ed, Vector2 pos1, Vector2 pos2);
void EraserToolUpdate(Editor* ed);