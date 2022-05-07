#include "help.h"
#include "../rlImGui/focusData.h"
#include "imgui.h"

void ShowHelp(FocusData* focus, bool* showHelp, bool modal)
{
    if (modal ? ImGui::BeginPopupModal("Help", showHelp) : true)
    {
        if (!modal)
        {
            ImGui::Begin("Help", showHelp);
        }
        focus->ObserveFocus();
        focus->isModal |= modal;
        if (ImGui::CollapsingHeader("Getting Started"))
        {
        }
        if (ImGui::CollapsingHeader("Editor Customization"))
        {
        }
        if (ImGui::CollapsingHeader("Tile Manipulation"))
        {
        }
        if (ImGui::CollapsingHeader("Entities"))
        {
        }
        if (ImGui::CollapsingHeader("Tileset Attributes"))
        {
        }
        if (modal)
        {
            ImGui::EndPopup();
        }
        else
        {
            ImGui::End();
        }
    }
}