#include "styleEditor.h"
#include <algorithm>
#include "editor.h"
#include "../rlImGui/utils.h"

StyleEditor::StyleEditor(Editor* ed)
{
    this->ed = ed;
}

void StyleEditor::ScanForThemes()
{

    // Delete old list.
    if (themes) DelImGuiStringList(themes, numThemes);

    // Fix window positioning.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Update theme list.
    std::vector<std::string> themeList = ReadFilesFromDir("object_data/themes");
    sort(themeList.begin(), themeList.end());
    themes = GenImGuiStringList(themeList, &numThemes);
    scanDirs = false;

    // Find current theme.
    currTheme = 0;
    for (int i = 0; i < themeList.size(); i++)
    {
        if (ed->settings.style.name == themeList[i])
        {
            currTheme = i;
            break;
        }
    }

}

void StyleEditor::DrawUI()
{

    // Window open check.
    if (!open) return;
    if (scanDirs) ScanForThemes();

    ImGui::Begin("Style Editor", &open);
    ed->focus.ObserveFocus();

    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Theme menu. It will add and remove files as needed.
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    if (ImGui::Combo("Current Style", &currTheme, themes, numThemes))
    {
        ed->settings.style.Load(themes[currTheme]);
        ed->settings.Save();
    }
    std::string oldName = ed->settings.style.name;
    if (ImGuiStringEdit("Style Name", &ed->settings.style.name))
    {
        ed->settings.style.Save();
        ed->settings.Save();
        scanDirs = true;
        remove(("object_data/themes/" + oldName + ".ini").c_str());
    }
    if (ed->settings.style.name != "New Theme" && ImGui::Button("New Theme"))
    {
        ed->settings.style.name = "New Theme";
        scanDirs = true;
        ed->settings.style.Save();
        ed->settings.Save();
    }
    if (numThemes > 1)
    {
        if (ed->settings.style.name != "New Theme") { ImGui::SameLine(); }
        if (ImGui::Button("Delete Theme"))
        {
            remove(("object_data/themes/" + ed->settings.style.name + ".ini").c_str());
            ed->settings.style.Load(themes[0]);
            scanDirs = true;
        }
    }

    // Copied from the ImGui demo and slightly modified for the editor.
    if (ImGui::Button("Save Style"))
    {
        ed->settings.style.style = ref_saved_style = style;
        ed->settings.style.Save();
    }
    ImGui::SameLine();
    if (ImGui::Button("Revert Style"))
        style = ed->settings.style.style;

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); ImGuiTooltip("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); ImGuiTooltip("Alignment applies when a selectable is larger than its text content.");
            ImGui::Text("Safe Area Padding");
            ImGui::SameLine(); ImGuiTooltip("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            ImGuiTooltip(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ed->settings.style.style.Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ed->settings.style.style.Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ed->settings.style.style.Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();
            ImGuiTooltip("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();
            ImGuiTooltip("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
    ImGui::End();

}