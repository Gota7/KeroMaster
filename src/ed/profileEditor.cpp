#include "profileEditor.h"
#include "editor.h"
#include "../px/profile.h"
#include "../rlImGui/dateChooser.h"
#include "../rlImGui/utils.h"

// Keycodes.
const char* WIN_KEYCODES[] =
{
    "UNKNOWN",
    "LBUTTON",
    "RBUTTON",
    "CANCEL",
    "MBUTTON",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "BACK",
    "TAB",
    "UNKNOWN",
    "UNKNOWN",
    "CLEAR",
    "RETURN",
    "UNKNOWN",
    "UNKNOWN",
    "SHIFT",
    "CONTROL",
    "MENU",
    "PAUSE",
    "CAPITAL",
    "KANA",
    "UNKNOWN",
    "JUNJA",
    "FINAL",
    "KANJI",
    "UNKNOWN",
    "ESCAPE",
    "CONVERT",
    "NONCONVERT",
    "ACCEPT",
    "MODECHANGE",
    "SPACE",
    "PRIOR",
    "NEXT",
    "END",
    "HOME",
    "LEFT",
    "UP",
    "RIGHT",
    "DOWN",
    "SELECT",
    "PRINT",
    "EXECUTE",
    "SNAPSHOT",
    "INSERT",
    "DELETE",
    "HELP",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "LWIN",
    "RWIN",
    "APPS",
    "UNKNOWN",
    "SLEEP",
    "NUMPAD0",
    "NUMPAD1",
    "NUMPAD2",
    "NUMPAD3",
    "NUMPAD4",
    "NUMPAD5",
    "NUMPAD6",
    "NUMPAD7",
    "NUMPAD8",
    "NUMPAD9",
    "MULTIPLY",
    "ADD",
    "SEPARATOR",
    "SUBTRACT",
    "DECIMAL",
    "DIVIDE",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "NUMLOCK",
    "SCROLL",
    "OEM_FJ_JISHO",
    "OEM_FJ_MASSHOU",
    "OEM_FJ_TOUROKU",
    "OEM_FJ_LOYA",
    "OEM_FJ_ROYA",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "LSHIFT",
    "RSHIFT",
    "LCONTROL",
    "RCONTROL",
    "LMENU",
    "RMENU",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "OEM_1",
    "OEM_PLUS",
    "OEM_COMMA",
    "OEM_MINUS",
    "OEM_PERIOD",
    "OEM_2",
    "OEM_3",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "OEM_4",
    "OEM_5",
    "OEM_6",
    "OEM_7",
    "OEM_8",
    "UNKNOWN",
    "OEM_AX",
    "OEM_102",
    "ICO_HELP",
    "ICO_00",
    "UNKNOWN",
    "ICO_CLEAR",
    "UNKNOWN",
    "UNKNOWN",
    "OEM_RESET",
    "OEM_JUMP",
    "OEM_PA1",
    "OEM_PA2",
    "OEM_PA3",
    "OEM_WSCTRL",
    "OEM_CUSEL",
    "OEM_ATTN",
    "OEM_FINISH",
    "OEM_COPY",
    "OEM_AUTO",
    "OEM_ENLW",
    "OEM_BACKTAB",
    "ATTN",
    "CRSEL",
    "EXSEL",
    "EREOF",
    "PLAY",
    "ZOOM",
    "NONAME",
    "PA1",
    "OEM_CLEAR"
};

ProfileEditor::ProfileEditor(Editor* ed)
{
    this->ed = ed;
    profile = &ed->profile;
}

void ProfileEditor::DrawUI()
{

    // Open check.
    if (!open) return;

    // Draw the editor.
    const int itemWidth = 150;
    ImGui::Begin("Profile Editor", &open);
    ed->focus.ObserveFocus();
    if (ImGui::BeginTabBar("Profile Tabs"))
    {
        if (ImGui::BeginTabItem("General"))
        {
            DrawGeneralSettings(itemWidth);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Screen"))
        {
            DrawScreenSettings(itemWidth);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Controls"))
        {
            DrawControlSettings(itemWidth);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Game"))
        {
            DrawGameProfile(itemWidth);
            ImGui::EndTabItem();
        }
        for (int i = 0; i < Profile::NUM_SAVES; i++)
        {
            if (ImGui::BeginTabItem(("Save " + std::to_string(i)).c_str()))
            {
                DrawSaveInfo(itemWidth, i);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

}

void ProfileEditor::DrawGeneralSettings(int itemWidth)
{

    // Region code.
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Region##GeneralSettings", &profile->region);

    // Last Idx.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Last Save File##GeneralSettings", ImGuiDataType_U8, &profile->lastProfileSave);
    ImGuiTooltip("ID of the last save file used.");
    ImGui::Separator();

    // Last date.
    tm time;
    time.tm_year = profile->lastYear - 1900;
    time.tm_mon = profile->lastMonth - 1;
    time.tm_mday = profile->lastDay;
    time.tm_hour = 0;
    time.tm_min = 0;
    time.tm_sec = 0;
    ImGui::PushItemWidth(itemWidth);
    if (ImGui::DateChooser("Last Play Date##GeneralSettings", time, "%m-%d-%Y"))
    {
        profile->lastYear = time.tm_year + 1900;
        profile->lastMonth = time.tm_mon + 1;
        profile->lastDay = time.tm_mday;
    }
    ImGuiTooltip("The date of when the user last played the game.");
    
    // Last time.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Last Play Hour", ImGuiDataType_U8, &profile->lastHour);
    ImGuiTooltip("The hour when the user last played the game.");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Last Play Minute", ImGuiDataType_U8, &profile->lastMinute);
    ImGuiTooltip("The minute when the user last played the game.");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Last Play Second", ImGuiDataType_U8, &profile->lastSecond);
    ImGuiTooltip("The second when the user last played the game.");

    // Save changes.
    if (ImGui::Button("Revert##GeneralSettings"))
    {
        profile->LoadRegion(ed->rsc);
        profile->LoadLastIdx(ed->rsc);
        profile->LoadLastTime(ed->rsc);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save##GeneralSettings"))
    {
        profile->SaveRegion(ed->rsc);
        profile->SaveLastIdx(ed->rsc);
        profile->SaveLastTime(ed->rsc);
    }

}

void ProfileEditor::DrawScreenSettings(int itemWidth)
{

    // Fullscreen.
    ImGui::PushItemWidth(itemWidth);
    ImGui::Checkbox("Fullscreen##ProfileScreenSettings", &profile->fullscreen);
    ImGui::Separator();

    // Rect.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Top Left Corner X Position##ProfileScreenSettings", ImGuiDataType_S32, &profile->topLeftCorner[0]);
    ImGuiTooltip("X position of the window.");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Top Left Corner Y Position##ProfileScreenSettings", ImGuiDataType_S32, &profile->topLeftCorner[1]);
    ImGuiTooltip("Y position of the window.");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Bottom Right Corner X Position##ProfileScreenSettings", ImGuiDataType_S32, &profile->bottomRightCorner[0]);
    ImGuiTooltip("Doesn't seem to matter?");
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Bottom Right Corner Y Position##ProfileScreenSettings", ImGuiDataType_S32, &profile->bottomRightCorner[1]);
    ImGuiTooltip("Doesn't seem to matter?");
    ImGui::Separator();

    // Size.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Screen Scale", ImGuiDataType_U8, &profile->screenSize);
    ImGuiTooltip("How many times to magnify the screen.");

    // Save changes.
    if (ImGui::Button("Revert##ProfileScreenSettings"))
    {
        profile->LoadFullscreen(ed->rsc);
        profile->LoadRect(ed->rsc);
        profile->LoadSize(ed->rsc);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save##ProfileScreenSettings"))
    {
        profile->SaveFullscreen(ed->rsc);
        profile->SaveRect(ed->rsc);
        profile->SaveSize(ed->rsc);
    }

}

void ProfileEditor::DrawControlSettings(int itemWidth)
{

    // Control interface.
    ImGui::Checkbox("Use Joystick##Controls", (bool*)&profile->useJoystick);
    ImGuiTooltip("If the joystick is the default input.");
    ImGui::Separator();

    // Key config.
    int tmpKey = profile->keyboardLeft;
    ImGui::Text("Keyboard Controls:");
    DrawControlOption(itemWidth, "Move Left##Controls", &profile->keyboardLeft, "Key to move the character left.");
    DrawControlOption(itemWidth, "Move Right##Controls", &profile->keyboardRight, "Key to move the character right.");
    DrawControlOption(itemWidth, "Look Up##Controls", &profile->keyboardUp, "Key to aim blaster up.");
    DrawControlOption(itemWidth, "Down Key##Controls", &profile->keyboardDown, "Key to move down ladders or menus.");
    DrawControlOption(itemWidth, "Jump##Controls", &profile->keyboardJump, "Key to make the character jump.");
    DrawControlOption(itemWidth, "Shoot##Controls", &profile->keyboardShoot, "Key to make the character shoot.");
    DrawControlOption(itemWidth, "Switch Weapon Left##Controls", &profile->keyboardWeaponSwitchLeft, "Key to use the left weapon.");
    DrawControlOption(itemWidth, "Switch Weapon Right##Controls", &profile->keyboardWeaponSwitchRight, "Key to use the right weapon.");
    DrawControlOption(itemWidth, "Menu##Controls", &profile->keyboardMenu, "Key to go to the main menu.");
    ImGui::Separator();
    ImGui::Text("Joystick Controls:");
    DrawControlOptionJoystick(itemWidth, "Jump##Controls2", &profile->joystickJump, "Button to make the character jump.");
    DrawControlOptionJoystick(itemWidth, "Shoot##Controls2", &profile->joystickShoot, "Button to make the character shoot.");
    DrawControlOptionJoystick(itemWidth, "Switch Weapon Left##Controls2", &profile->joystickWeaponSwitchLeft, "Button to use the left weapon.");
    DrawControlOptionJoystick(itemWidth, "Switch Weapon Right##Controls2", &profile->joystickWeaponSwitchRight, "Button to use the right weapon.");
    DrawControlOptionJoystick(itemWidth, "Menu##Controls2", &profile->joystickMenu, "Button to go to the main menu.");

    // Save controls.
    if (ImGui::Button("Revert##ProfileScreenSettings"))
    {
        profile->LoadControlInterfaceMode(ed->rsc);
        profile->LoadKeyConfig(ed->rsc);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save##ProfileScreenSettings"))
    {
        profile->SaveControlInterfaceMode(ed->rsc);
        profile->SaveKeyConfig(ed->rsc);
    }

}

void ProfileEditor::DrawControlOption(int itemWidth, const char* name, u8* field, const char* desc)
{
    ImGui::PushItemWidth(itemWidth);
    int tmpField = *field;
    if (ImGui::Combo(name, &tmpField, WIN_KEYCODES, IM_ARRAYSIZE(WIN_KEYCODES)))
    {
        *field = (u8)tmpField;
    }
    ImGuiTooltip(desc);
}

void ProfileEditor::DrawControlOptionJoystick(int itemWidth, const char* name, u8* field, const char* desc)
{
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar(name, ImGuiDataType_U8, field);
    ImGuiTooltip(desc);
}

void ProfileEditor::DrawGameProfile(int itemWidth)
{

    // Dummy.
    ImGui::Text("Dummy");

}

void ProfileEditor::DrawSaveInfo(int itemWidth, int save)
{

    // Dummy.
    ImGui::Text("Dummy");

}