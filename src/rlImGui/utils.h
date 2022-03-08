#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "../types.h"

// Read a directory of files.
std::vector<std::string> ReadFilesFromDir(std::string path, bool includeExt = false);

// Convert a string reference to a char pointer.
char* ConvertStrRef(const std::string& s);

// Convert a vector of strings into something ImGui can use.
char** GenImGuiStringList(std::vector<std::string>& strings, int* outStringCount);

// Delete an ImGui string list.
void DelImGuiStringList(char** ptr, int stringCount);

// Edit a number with arrows on the side.
void ImGuiNumEdit(std::string name, s64& data, size_t min, size_t max, std::string format = "%d");

// Show a tooltip if hovered.
void ImGuiTooltip(std::string tip);

// Edit a string.
bool ImGuiStringEdit(const char* label, std::string* toEdit);

// Edit a color.
void ImGuiColorEdit(const char* label, u8* toEdit);