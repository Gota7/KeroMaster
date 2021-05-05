#pragma once
#include <imgui.h>
#include <string>
#include "../types.h"
using namespace std;

// Edit a number with arrows on the side.
void ImGuiNumEdit(string name, s64& data, size_t min, size_t max, string format = "%d");

// Show a tooltip if hovered.
void ImGuiTooltip(string tip);

// Edit a string.
void ImGuiStringEdit(const char* label, string* toEdit);