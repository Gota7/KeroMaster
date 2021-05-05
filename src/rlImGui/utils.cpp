#include "utils.h"

void ImGuiNumEdit(string name, s64& data, size_t min, size_t max, string format)
{
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton(("##" + name + "left").c_str(), ImGuiDir_Left) && data > min) { data--; }
    ImGui::SameLine(0.0f, spacing);
    ImGui::Text(format.c_str(), data);
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton(("##right" + name + "left").c_str(), ImGuiDir_Right) && data < max) { data++; }
    ImGui::PopButtonRepeat();
}

void ImGuiTooltip(string tip)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::SetTooltip(tip.c_str());
        ImGui::EndTooltip();
    }
}

void ImGuiStringEdit(const char* label, string* toEdit)
{
    char buf[128];
    strcpy(buf, &(*toEdit)[0]);
    ImGui::InputText(label, buf, 128);
    *toEdit = string(buf);
}