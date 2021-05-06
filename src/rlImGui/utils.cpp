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

void ImGuiColorEdit(const char* label, u8* toEdit)
{
    float tmp[3];
    tmp[0] = toEdit[0] / (float)255;
    tmp[1] = toEdit[1] / (float)255;
    tmp[2] = toEdit[2] / (float)255;
    if (ImGui::ColorEdit3(label, tmp)) {
        toEdit[0] = (u8)(tmp[0] * 255);
        toEdit[1] = (u8)(tmp[1] * 255);
        toEdit[2] = (u8)(tmp[2] * 255);
    }
}