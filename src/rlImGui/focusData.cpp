#include "focusData.h"

void FocusData::ObserveFocus()
{
    windowFocused |= ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
    mouseInWindow |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
}

void FocusData::Update()
{
    if (!windowFocused && prevWindowFocused)
    {
        isFirstUnfocus = true;
    }
    else
    {
        isFirstUnfocus = false;
    }
    prevWindowFocused = windowFocused;
    windowFocused = false;
    mouseInWindow = false;
    isModal = false;
}