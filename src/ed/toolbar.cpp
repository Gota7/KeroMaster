#include "toolbar.h"

#include "editorNew.h"
#include "raylib.h"
#include <imgui.h>

Toolbar::Toolbar(EditorNew* ed)
{
    this->ed = ed;
}

void Toolbar::DrawUI()
{
    ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::SetWindowPos(ImVec2(10.0f, 30.0f), ImGuiCond_FirstUseEver);

    ed->focus.ObserveFocus();

    int index = 0;
    auto ToolButton = [&](const char* label, ToolItem tool)
    {
        bool active = ed->currTool == tool;

        if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

        if (index++ > 0) ImGui::SameLine();
        if (ImGui::Button(label))
        {
            ed->currTool = tool;
        }

        if (active) ImGui::PopStyleColor();
    };

    auto ToggleButton = [&](const char* label, bool active, bool* rightToggle) -> bool
    {
        bool action = false;
        if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

        if (index++ > 0) ImGui::SameLine();
        if (ImGui::Button(label))
        {
            action = true;
        }
        else if (ImGui::IsItemClicked(1))
        {
            if (rightToggle != nullptr)
            {
                *rightToggle = !*rightToggle;
            }
        }

        if (active) ImGui::PopStyleColor();
        return action;
    };

    auto Tooltip = [](const char* message)
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::SetTooltip("%s", message);
            ImGui::EndTooltip();
        }
    };

    // Add tool buttons.
    for (int i = 0; i < (int)ToolItem::NumTools; i++)
    {
        ToolButton(Tools::toolNames[i], (ToolItem)i);
    }

    // Zoom setting.
    ImGui::SameLine();
    float newZoom = ed->cam.zoom;
    ImGui::SliderFloat("", &newZoom, ed->MIN_ZOOM, ed->MAX_ZOOM, "Scale: %.2fx", ImGuiSliderFlags_NoRoundToFormat);
    if (ed->cam.zoom != newZoom)
    {
        ed->Zoom({ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f }, newZoom, false);
    }

    // Grid setting.
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &ed->settings.showGrid);

    //Map layer management.
    if (ToggleButton("FG", ed->currentLayer == (int)MapLayer::FG, &ed->settings.viewLayers[(int)MapLayer::FG]))
    {
        ed->currentLayer = (int)MapLayer::FG;
        ed->settings.viewLayers[(int)MapLayer::FG] = true;
    }
    Tooltip("Foreground Layer\nRight click to toggle visibility.");

    if (ToggleButton("MG", ed->currentLayer == (int)MapLayer::MG, &ed->settings.viewLayers[(int)MapLayer::MG]))
    {
        ed->currentLayer = (int)MapLayer::MG;
        ed->settings.viewLayers[(int)MapLayer::MG] = true;
    }
    Tooltip("Middleground Layer\nRight click to toggle visibility.");

    if (ToggleButton("BG", ed->currentLayer == (int)MapLayer::BG, &ed->settings.viewLayers[(int)MapLayer::BG]))
    {
        ed->currentLayer = (int)MapLayer::BG;
        ed->settings.viewLayers[(int)MapLayer::BG] = true;
    }
    Tooltip("Background Layer\nRight click to toggle visibility.");

    // Entity placing tool.
    if (ed->currTool == ToolItem::EntityHand)
    {
        ImGui::SameLine();
        ImGui::Combo("Place Entity Id", &ed->placeEntityId, ed->entityEditor.entityListing, ed->entityEditor.numEntities);
        ImGui::SameLine();
        if (ToggleButton("+", ed->isPlacingEntity, nullptr))
        {
            ed->isPlacingEntity = !ed->isPlacingEntity;
        }
        Tooltip("Click after hitting to place an entity or again to cancel.\nHolding shift while placing will allow you to place multiple.\nPressing the delete key will delete the selected entity.");
    }
    ImGui::End();

}