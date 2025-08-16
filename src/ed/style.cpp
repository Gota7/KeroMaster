#include "style.h"
#include "inicpp.h"
#include "settings.h"
#include "../rlImGui/rlImGui.h"
#include "themes/CatAndFrog.h"
#include "themes/PurplePower.h"

using ApplyStyleFunc = void (*)(ImGuiStyle *);
using BuiltinStyleMap = std::map<std::string, ApplyStyleFunc>;

static BuiltinStyleMap builtin_styles = {
    {"Cat & Frog", ApplyStyle_CatAndFrog},
    {"ImGui Dark", ImGui::StyleColorsDark},
    {"ImGui Light", ImGui::StyleColorsLight},
    {"ImGui Classic", ImGui::StyleColorsClassic},
    {"Purple Power", ApplyStyle_PurplePower},
};

template <typename T>
static void DeserializeImguiStyleProp(T &value, ini::IniSection &section, const std::string &name)
{
    try
    {
        if constexpr (std::is_same_v<T, ImVec2>)
        {
            value.x = section[name + "X"].as<float>();
            value.y = section[name + "Y"].as<float>();
        }
        else if constexpr (std::is_same_v<T, ImVec4>)
        {
            value.x = section[name + "R"].as<float>();
            value.y = section[name + "G"].as<float>();
            value.z = section[name + "B"].as<float>();
            value.w = section[name + "A"].as<float>();
        }
        else if constexpr (std::is_same_v<T, ImGuiDir>)
        {
            value = static_cast<ImGuiDir>(section[name].as<int>());
        }
        else if constexpr (std::is_same_v<T, ImGuiTreeNodeFlags>)
        {
            auto v = static_cast<ImGuiTreeNodeFlags>(section[name].as<int>());
            if (v == ImGuiTreeNodeFlags_DrawLinesNone ||
                v == ImGuiTreeNodeFlags_DrawLinesFull ||
                v == ImGuiTreeNodeFlags_DrawLinesToNodes)
            {
                value = v;
            }
        }
        else
        {
            value = section[name].as<T>();
        }
    }
    catch (const std::exception &)
    {
        // do nothing if the key is not found or deserialization fails
    }
}

template <typename T>
static void SerializeImguiStyleProp(T &value, ini::IniSection &section, const std::string &name)
{
    if constexpr (std::is_same_v<T, ImVec2>)
    {
        section[name + "X"] = value.x;
        section[name + "Y"] = value.y;
    }
    else if constexpr (std::is_same_v<T, ImVec4>)
    {
        section[name + "R"] = value.x;
        section[name + "G"] = value.y;
        section[name + "B"] = value.z;
        section[name + "A"] = value.w;
    }
    else if constexpr (std::is_same_v<T, ImGuiDir>)
    {
        section[name] = static_cast<int>(value);
    }
    else if constexpr (std::is_same_v<T, ImGuiTreeNodeFlags>)
    {
        section[name] = static_cast<int>(value);
    }
    else
    {
        section[name] = value;
    }
}

static std::array<ImGuiCol, 53> LEGACY_COLOR_INDEX_MAPPING{
    ImGuiCol_Text,
    ImGuiCol_TextDisabled,
    ImGuiCol_WindowBg,
    ImGuiCol_ChildBg,
    ImGuiCol_PopupBg,
    ImGuiCol_Border,
    ImGuiCol_BorderShadow,
    ImGuiCol_FrameBg,
    ImGuiCol_FrameBgHovered,
    ImGuiCol_FrameBgActive,
    ImGuiCol_TitleBg,
    ImGuiCol_TitleBgActive,
    ImGuiCol_TitleBgCollapsed,
    ImGuiCol_MenuBarBg,
    ImGuiCol_ScrollbarBg,
    ImGuiCol_ScrollbarGrab,
    ImGuiCol_ScrollbarGrabHovered,
    ImGuiCol_ScrollbarGrabActive,
    ImGuiCol_CheckMark,
    ImGuiCol_SliderGrab,
    ImGuiCol_SliderGrabActive,
    ImGuiCol_Button,
    ImGuiCol_ButtonHovered,
    ImGuiCol_ButtonActive,
    ImGuiCol_Header,
    ImGuiCol_HeaderHovered,
    ImGuiCol_HeaderActive,
    ImGuiCol_Separator,
    ImGuiCol_SeparatorHovered,
    ImGuiCol_SeparatorActive,
    ImGuiCol_ResizeGrip,
    ImGuiCol_ResizeGripHovered,
    ImGuiCol_ResizeGripActive,
    ImGuiCol_Tab,
    ImGuiCol_TabHovered,
    ImGuiCol_TabActive,
    ImGuiCol_TabUnfocused,
    ImGuiCol_TabUnfocusedActive,
    ImGuiCol_PlotLines,
    ImGuiCol_PlotLinesHovered,
    ImGuiCol_PlotHistogram,
    ImGuiCol_PlotHistogramHovered,
    ImGuiCol_TableHeaderBg,
    ImGuiCol_TableBorderStrong,
    ImGuiCol_TableBorderLight,
    ImGuiCol_TableRowBg,
    ImGuiCol_TableRowBgAlt,
    ImGuiCol_TextSelectedBg,
    ImGuiCol_DragDropTarget,
    ImGuiCol_NavHighlight,
    ImGuiCol_NavWindowingHighlight,
    ImGuiCol_NavWindowingDimBg,
    ImGuiCol_ModalWindowDimBg,
};

// ^.*\s(.*?);.*$
// DESERIALIZE_IMGUI_STYLE_PROP($1);
#define DESERIALIZE_IMGUI_STYLE_PROP(NAME) DeserializeImguiStyleProp(style.NAME, theme, #NAME)
#define SERIALIZE_IMGUI_STYLE_PROP(NAME) SerializeImguiStyleProp(style.NAME, theme, #NAME)

void EditorStyle::Load(std::string name, Settings *settings)
{
    auto it = builtin_styles.end();
    if (name == "default")
    {
        it = builtin_styles.begin();
        printf("Default theme applied.\n");
    }
    else
    {
        printf("Using built-in theme: %s\n", name.c_str());
        it = builtin_styles.find(name);
    }

    if (it != builtin_styles.end())
    {
        auto &[name, func] = *it;
        this->name = name;
        func(&style);
    }

    ini::IniFile t = ini::IniFile(settings->themesPath + "/" + name + ".ini");
    auto &theme = t["Theme"];

    DESERIALIZE_IMGUI_STYLE_PROP(Alpha);
    DESERIALIZE_IMGUI_STYLE_PROP(DisabledAlpha);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowBorderHoverPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowMinSize);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowTitleAlign);
    DESERIALIZE_IMGUI_STYLE_PROP(WindowMenuButtonPosition);
    DESERIALIZE_IMGUI_STYLE_PROP(ChildRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(ChildBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(PopupRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(PopupBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(FramePadding);
    DESERIALIZE_IMGUI_STYLE_PROP(FrameRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(FrameBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(ItemSpacing);
    DESERIALIZE_IMGUI_STYLE_PROP(ItemInnerSpacing);
    DESERIALIZE_IMGUI_STYLE_PROP(CellPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(TouchExtraPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(IndentSpacing);
    DESERIALIZE_IMGUI_STYLE_PROP(ColumnsMinSpacing);
    DESERIALIZE_IMGUI_STYLE_PROP(ScrollbarSize);
    DESERIALIZE_IMGUI_STYLE_PROP(ScrollbarRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(GrabMinSize);
    DESERIALIZE_IMGUI_STYLE_PROP(GrabRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(LogSliderDeadzone);
    DESERIALIZE_IMGUI_STYLE_PROP(ImageBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(TabRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(TabBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(TabMinWidthBase);
    DESERIALIZE_IMGUI_STYLE_PROP(TabMinWidthShrink);
    DESERIALIZE_IMGUI_STYLE_PROP(TabCloseButtonMinWidthSelected);
    DESERIALIZE_IMGUI_STYLE_PROP(TabCloseButtonMinWidthUnselected);
    DESERIALIZE_IMGUI_STYLE_PROP(TabBarBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(TabBarOverlineSize);
    DESERIALIZE_IMGUI_STYLE_PROP(TableAngledHeadersAngle);
    DESERIALIZE_IMGUI_STYLE_PROP(TableAngledHeadersTextAlign);
    DESERIALIZE_IMGUI_STYLE_PROP(TreeLinesFlags);
    DESERIALIZE_IMGUI_STYLE_PROP(TreeLinesSize);
    DESERIALIZE_IMGUI_STYLE_PROP(TreeLinesRounding);
    DESERIALIZE_IMGUI_STYLE_PROP(ColorButtonPosition);
    DESERIALIZE_IMGUI_STYLE_PROP(ButtonTextAlign);
    DESERIALIZE_IMGUI_STYLE_PROP(SelectableTextAlign);
    DESERIALIZE_IMGUI_STYLE_PROP(SeparatorTextBorderSize);
    DESERIALIZE_IMGUI_STYLE_PROP(SeparatorTextAlign);
    DESERIALIZE_IMGUI_STYLE_PROP(SeparatorTextPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(DisplayWindowPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(DisplaySafeAreaPadding);
    DESERIALIZE_IMGUI_STYLE_PROP(MouseCursorScale);
    DESERIALIZE_IMGUI_STYLE_PROP(AntiAliasedLines);
    DESERIALIZE_IMGUI_STYLE_PROP(AntiAliasedLinesUseTex);
    DESERIALIZE_IMGUI_STYLE_PROP(AntiAliasedFill);
    DESERIALIZE_IMGUI_STYLE_PROP(CurveTessellationTol);
    DESERIALIZE_IMGUI_STYLE_PROP(CircleTessellationMaxError);

    // Load legacy colors
    for (int i = 0; i < LEGACY_COLOR_INDEX_MAPPING.size(); i++)
    {
        // auto& color = style.Colors[LEGACY_COLOR_INDEX_MAPPING[i]];
        try
        {
            ImVec4 color{};
            color.x = theme["ColorR_" + std::to_string(i)].as<float>();
            color.y = theme["ColorG_" + std::to_string(i)].as<float>();
            color.z = theme["ColorB_" + std::to_string(i)].as<float>();
            color.w = theme["ColorA_" + std::to_string(i)].as<float>();
            style.Colors[LEGACY_COLOR_INDEX_MAPPING[i]] = color;
        }
        catch (const std::exception &)
        {
            // ...
        }
    }

    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        auto colorName = std::string("Color") + ImGui::GetStyleColorName(i);
        DeserializeImguiStyleProp(style.Colors[i], theme, colorName);
    }

    ImGui::GetStyle() = style;
}

void EditorStyle::Save(Settings *settings)
{
    if (name == "default")
    {
        return;
    }
    ini::IniFile t;
    auto &theme = t["Theme"];
    theme["StyleConfigVersion"] = 2;

    SERIALIZE_IMGUI_STYLE_PROP(Alpha);
    SERIALIZE_IMGUI_STYLE_PROP(DisabledAlpha);
    SERIALIZE_IMGUI_STYLE_PROP(WindowPadding);
    SERIALIZE_IMGUI_STYLE_PROP(WindowRounding);
    SERIALIZE_IMGUI_STYLE_PROP(WindowBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(WindowBorderHoverPadding);
    SERIALIZE_IMGUI_STYLE_PROP(WindowMinSize);
    SERIALIZE_IMGUI_STYLE_PROP(WindowTitleAlign);
    SERIALIZE_IMGUI_STYLE_PROP(WindowMenuButtonPosition);
    SERIALIZE_IMGUI_STYLE_PROP(ChildRounding);
    SERIALIZE_IMGUI_STYLE_PROP(ChildBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(PopupRounding);
    SERIALIZE_IMGUI_STYLE_PROP(PopupBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(FramePadding);
    SERIALIZE_IMGUI_STYLE_PROP(FrameRounding);
    SERIALIZE_IMGUI_STYLE_PROP(FrameBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(ItemSpacing);
    SERIALIZE_IMGUI_STYLE_PROP(ItemInnerSpacing);
    SERIALIZE_IMGUI_STYLE_PROP(CellPadding);
    SERIALIZE_IMGUI_STYLE_PROP(TouchExtraPadding);
    SERIALIZE_IMGUI_STYLE_PROP(IndentSpacing);
    SERIALIZE_IMGUI_STYLE_PROP(ColumnsMinSpacing);
    SERIALIZE_IMGUI_STYLE_PROP(ScrollbarSize);
    SERIALIZE_IMGUI_STYLE_PROP(ScrollbarRounding);
    SERIALIZE_IMGUI_STYLE_PROP(GrabMinSize);
    SERIALIZE_IMGUI_STYLE_PROP(GrabRounding);
    SERIALIZE_IMGUI_STYLE_PROP(LogSliderDeadzone);
    SERIALIZE_IMGUI_STYLE_PROP(ImageBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(TabRounding);
    SERIALIZE_IMGUI_STYLE_PROP(TabBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(TabMinWidthBase);
    SERIALIZE_IMGUI_STYLE_PROP(TabMinWidthShrink);
    SERIALIZE_IMGUI_STYLE_PROP(TabCloseButtonMinWidthSelected);
    SERIALIZE_IMGUI_STYLE_PROP(TabCloseButtonMinWidthUnselected);
    SERIALIZE_IMGUI_STYLE_PROP(TabBarBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(TabBarOverlineSize);
    SERIALIZE_IMGUI_STYLE_PROP(TableAngledHeadersAngle);
    SERIALIZE_IMGUI_STYLE_PROP(TableAngledHeadersTextAlign);
    SERIALIZE_IMGUI_STYLE_PROP(TreeLinesFlags);
    SERIALIZE_IMGUI_STYLE_PROP(TreeLinesSize);
    SERIALIZE_IMGUI_STYLE_PROP(TreeLinesRounding);
    SERIALIZE_IMGUI_STYLE_PROP(ColorButtonPosition);
    SERIALIZE_IMGUI_STYLE_PROP(ButtonTextAlign);
    SERIALIZE_IMGUI_STYLE_PROP(SelectableTextAlign);
    SERIALIZE_IMGUI_STYLE_PROP(SeparatorTextBorderSize);
    SERIALIZE_IMGUI_STYLE_PROP(SeparatorTextAlign);
    SERIALIZE_IMGUI_STYLE_PROP(SeparatorTextPadding);
    SERIALIZE_IMGUI_STYLE_PROP(DisplayWindowPadding);
    SERIALIZE_IMGUI_STYLE_PROP(DisplaySafeAreaPadding);
    SERIALIZE_IMGUI_STYLE_PROP(MouseCursorScale);
    SERIALIZE_IMGUI_STYLE_PROP(AntiAliasedLines);
    SERIALIZE_IMGUI_STYLE_PROP(AntiAliasedLinesUseTex);
    SERIALIZE_IMGUI_STYLE_PROP(AntiAliasedFill);
    SERIALIZE_IMGUI_STYLE_PROP(CurveTessellationTol);
    SERIALIZE_IMGUI_STYLE_PROP(CircleTessellationMaxError);
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        auto colorName = std::string("Color") + ImGui::GetStyleColorName(i);
        SerializeImguiStyleProp(style.Colors[i], theme, colorName);
    }

    t.save(settings->themesPath + "/" + name + ".ini");
}
