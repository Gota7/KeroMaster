#include "focusData.h"

void FocusData::ObserveFocus()
{
    windowFocused |= ImGui::IsWindowFocused();
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    mouseInWindow |= mouseX > ImGui::GetWindowPos().x && \
    mouseX < ImGui::GetWindowPos().x + ImGui::GetWindowSize().x && \
    mouseY > ImGui::GetWindowPos().y && \
    mouseY < ImGui::GetWindowPos().y + ImGui::GetWindowSize().y;
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