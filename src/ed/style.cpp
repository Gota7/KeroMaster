#include "style.h"
#include "inicpp.h"
#include "../rlImGui/rlImGui.h"

void EditorStyle::Load(std::string name)
{
    this->name = name;
    if (name == "default")
    {
        ImGui::StyleColorsClassic(&style);
        return;
    }
    ini::IniFile t = ini::IniFile("object_data/themes/" + name + ".ini");
    style.Alpha = t["Theme"]["Alpha"].as<float>();
    style.AntiAliasedFill = t["Theme"]["AntiAliasedFill"].as<bool>();
    style.AntiAliasedLines = t["Theme"]["AntiAliasedLines"].as<bool>();
    style.AntiAliasedLinesUseTex = t["Theme"]["AntiAliasedLinesUseTex"].as<bool>();
    style.ButtonTextAlign.x = t["Theme"]["ButtonTextAlignX"].as<float>();
    style.ButtonTextAlign.y = t["Theme"]["ButtonTextAlignY"].as<float>();
    style.CellPadding.x = t["Theme"]["CellPaddingX"].as<float>();
    style.CellPadding.y = t["Theme"]["CellPaddingY"].as<float>();
    style.ChildBorderSize = t["Theme"]["ChildBorderSize"].as<float>();
    style.ChildRounding = t["Theme"]["ChildRounding"].as<float>();
    style.CircleTessellationMaxError = t["Theme"]["CircleTessellationMaxError"].as<float>();
    style.ColorButtonPosition = (ImGuiDir)t["Theme"]["ColorButtonPosition"].as<int>();
    for (int i = 0; i < 53; i++)
    {
        style.Colors[i].x = t["Theme"]["ColorR_" + std::to_string(i)].as<float>();
        style.Colors[i].y = t["Theme"]["ColorG_" + std::to_string(i)].as<float>();
        style.Colors[i].z = t["Theme"]["ColorB_" + std::to_string(i)].as<float>();
        style.Colors[i].w = t["Theme"]["ColorA_" + std::to_string(i)].as<float>();
    }
    style.ColumnsMinSpacing = t["Theme"]["ColumnsMinSpacing"].as<float>();
    style.CurveTessellationTol = t["Theme"]["CurveTessellationTol"].as<float>();
    style.DisplaySafeAreaPadding.x = t["Theme"]["DisplaySafeAreaPaddingX"].as<float>();
    style.DisplaySafeAreaPadding.y = t["Theme"]["DisplaySafeAreaPaddingY"].as<float>();
    style.DisplayWindowPadding.x = t["Theme"]["DisplayWindowPaddingX"].as<float>();
    style.DisplayWindowPadding.y = t["Theme"]["DisplayWindowPaddingY"].as<float>();
    style.FrameBorderSize = t["Theme"]["FrameBorderSize"].as<float>();
    style.FramePadding.x = t["Theme"]["FramePaddingX"].as<float>();
    style.FramePadding.y = t["Theme"]["FramePaddingY"].as<float>();
    style.FrameRounding = t["Theme"]["FrameRounding"].as<float>();
    style.GrabMinSize = t["Theme"]["GrabMinSize"].as<float>();
    style.GrabRounding = t["Theme"]["GrabRounding"].as<float>();
    style.IndentSpacing = t["Theme"]["IndentSpacing"].as<float>();
    style.ItemInnerSpacing.x = t["Theme"]["ItemInnerSpacingX"].as<float>();
    style.ItemInnerSpacing.y = t["Theme"]["ItemInnerSpacingY"].as<float>();
    style.ItemSpacing.x = t["Theme"]["ItemSpacingX"].as<float>();
    style.ItemSpacing.y = t["Theme"]["ItemSpacingY"].as<float>();
    style.LogSliderDeadzone = t["Theme"]["LogSliderDeadzone"].as<float>();
    style.MouseCursorScale = t["Theme"]["MouseCursorScale"].as<float>();
    style.PopupBorderSize = t["Theme"]["PopupBorderSize"].as<float>();
    style.PopupRounding = t["Theme"]["PopupRounding"].as<float>();
    style.ScrollbarRounding = t["Theme"]["ScrollbarRounding"].as<float>();
    style.ScrollbarSize = t["Theme"]["ScrollbarSize"].as<float>();
    style.SelectableTextAlign.x = t["Theme"]["SelectableTextAlignX"].as<float>();
    style.SelectableTextAlign.y = t["Theme"]["SelectableTextAlignY"].as<float>();
    style.TabBorderSize = t["Theme"]["TabBorderSize"].as<float>();
    style.TabMinWidthForCloseButton = t["Theme"]["TabMinWidthForCloseButton"].as<float>();
    style.TabRounding = t["Theme"]["TabRounding"].as<float>();
    style.TouchExtraPadding.x = t["Theme"]["TouchExtraPaddingX"].as<float>();
    style.TouchExtraPadding.y = t["Theme"]["TouchExtraPaddingY"].as<float>();
    style.WindowBorderSize = t["Theme"]["WindowBorderSize"].as<float>();
    style.WindowMenuButtonPosition = (ImGuiDir)t["Theme"]["WindowMenuButtonPosition"].as<int>();
    style.WindowMinSize.x = t["Theme"]["WindowMinSizeX"].as<float>();
    style.WindowMinSize.y = t["Theme"]["WindowMinSizeY"].as<float>();
    style.WindowPadding.x = t["Theme"]["WindowPaddingX"].as<float>();
    style.WindowPadding.y = t["Theme"]["WindowPaddingY"].as<float>();
    style.WindowRounding = t["Theme"]["WindowRounding"].as<float>();
    style.WindowTitleAlign.x = t["Theme"]["WindowTitleAlignX"].as<float>();
    style.WindowTitleAlign.y = t["Theme"]["WindowTitleAlignY"].as<float>();
    ImGui::GetStyle() = style;
}

void EditorStyle::Save()
{
    if (name == "default")
    {
        return;
    }
    ini::IniFile t;
    t["Theme"]["Alpha"] = style.Alpha;
    t["Theme"]["AntiAliasedFill"] = style.AntiAliasedFill;
    t["Theme"]["AntiAliasedLines"] = style.AntiAliasedLines;
    t["Theme"]["AntiAliasedLinesUseTex"] = style.AntiAliasedLinesUseTex;
    t["Theme"]["ButtonTextAlignX"] = style.ButtonTextAlign.x;
    t["Theme"]["ButtonTextAlignY"] = style.ButtonTextAlign.y;
    t["Theme"]["CellPaddingX"] = style.CellPadding.x;
    t["Theme"]["CellPaddingY"] = style.CellPadding.y;
    t["Theme"]["ChildBorderSize"] = style.ChildBorderSize;
    t["Theme"]["ChildRounding"] = style.ChildRounding;
    t["Theme"]["CircleTessellationMaxError"] = style.CircleTessellationMaxError;
    t["Theme"]["ColorButtonPosition"] = (int)style.ColorButtonPosition;
    for (int i = 0; i < 53; i++)
    {
        t["Theme"]["ColorR_" + std::to_string(i)] = style.Colors[i].x;
        t["Theme"]["ColorG_" + std::to_string(i)] = style.Colors[i].y;
        t["Theme"]["ColorB_" + std::to_string(i)] = style.Colors[i].z;
        t["Theme"]["ColorA_" + std::to_string(i)] = style.Colors[i].w;
    }
    t["Theme"]["ColumnsMinSpacing"] = style.ColumnsMinSpacing;
    t["Theme"]["CurveTessellationTol"] = style.CurveTessellationTol;
    t["Theme"]["DisplaySafeAreaPaddingX"] = style.DisplaySafeAreaPadding.x;
    t["Theme"]["DisplaySafeAreaPaddingY"] = style.DisplaySafeAreaPadding.y;
    t["Theme"]["DisplayWindowPaddingX"] = style.DisplayWindowPadding.x;
    t["Theme"]["DisplayWindowPaddingY"] = style.DisplayWindowPadding.y;
    t["Theme"]["FrameBorderSize"] = style.FrameBorderSize;
    t["Theme"]["FramePaddingX"] = style.FramePadding.x;
    t["Theme"]["FramePaddingY"] = style.FramePadding.y;
    t["Theme"]["FrameRounding"] = style.FrameRounding;
    t["Theme"]["GrabMinSize"] = style.GrabMinSize;
    t["Theme"]["GrabRounding"] = style.GrabRounding;
    t["Theme"]["IndentSpacing"] = style.IndentSpacing;
    t["Theme"]["ItemInnerSpacingX"] = style.ItemInnerSpacing.x;
    t["Theme"]["ItemInnerSpacingY"] = style.ItemInnerSpacing.y;
    t["Theme"]["ItemSpacingX"] = style.ItemSpacing.x;
    t["Theme"]["ItemSpacingY"] = style.ItemSpacing.y;
    t["Theme"]["LogSliderDeadzone"] = style.LogSliderDeadzone;
    t["Theme"]["MouseCursorScale"] = style.MouseCursorScale;
    t["Theme"]["PopupBorderSize"] = style.PopupBorderSize;
    t["Theme"]["PopupRounding"] = style.PopupRounding;
    t["Theme"]["ScrollbarRounding"] = style.ScrollbarRounding;
    t["Theme"]["ScrollbarSize"] = style.ScrollbarSize;
    t["Theme"]["SelectableTextAlignX"] = style.SelectableTextAlign.x;
    t["Theme"]["SelectableTextAlignY"] = style.SelectableTextAlign.y;
    t["Theme"]["TabBorderSize"] = style.TabBorderSize;
    t["Theme"]["TabMinWidthForCloseButton"] = style.TabMinWidthForCloseButton;
    t["Theme"]["TabRounding"] = style.TabRounding;
    t["Theme"]["TouchExtraPaddingX"] = style.TouchExtraPadding.x;
    t["Theme"]["TouchExtraPaddingY"] = style.TouchExtraPadding.y;
    t["Theme"]["WindowBorderSize"] = style.WindowBorderSize;
    t["Theme"]["WindowMenuButtonPosition"] = (int)style.WindowMenuButtonPosition;
    t["Theme"]["WindowMinSizeX"] = style.WindowMinSize.x;
    t["Theme"]["WindowMinSizeY"] = style.WindowMinSize.y;
    t["Theme"]["WindowPaddingX"] = style.WindowPadding.x;
    t["Theme"]["WindowPaddingY"] = style.WindowPadding.y;
    t["Theme"]["WindowRounding"] = style.WindowRounding;
    t["Theme"]["WindowTitleAlignX"] = style.WindowTitleAlign.x;
    t["Theme"]["WindowTitleAlignY"] = style.WindowTitleAlign.y;
    t.save("object_data/themes/" + name + ".ini");
}