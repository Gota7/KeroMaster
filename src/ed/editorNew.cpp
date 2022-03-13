#include "editorNew.h"
#include "../rlImGui/rlImGui.h"
#include "../rlImGui/utils.h"
#include "imgui.h"
#include "raylib.h"

void EditorNew::Init()
{

    // Load the settings.
    settings.Load();

    // Then set up the main window.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | (settings.maximized ? FLAG_WINDOW_MAXIMIZED : 0));
    InitWindow(settings.width, settings.height, "Kero Master");
    icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    SetupRLImGui(true);
    style.Load(settings.currStyle);

}

int EditorNew::EditorLoop()
{

    // Main loop.
    while (!WindowShouldClose())
    {

        // Begin drawing.
        BeginDrawing();

        // Something went wrong, need to show the settings window.
        if (settings.show)
        {
            ClearBackground(fadeColor);
            BeginRLImGui();
            ImGui::OpenPopup("Editor Settings");
            settings.DrawUI(this);
        }

        // Draw like normal.
        else
        {
            Draw();
            BeginRLImGui();
            DrawUI();
        }

        // ImGui demo debug.
        ImGui::ShowDemoWindow();
        
        // Finish drawing.
        EndRLImGui();
        EndDrawing();
        Update();

        // Fullscreen check.
        /*if (IsKeyPressed(KEY_F4) || doFullscreen)
        {
            if (!IsWindowFullscreen() && !doFullscreen)
            {
                MaximizeWindow();
                timer = GetTime();
                doFullscreen = true;
            }
            if (GetTime() > e.timer + 0.1)
            {
                ToggleFullscreen();
                doFullscreen = false;
            }
        }*/

    }

    // Unload everything.
    UnloadImage(icon);
    ShutdownRLImGui();
    CloseWindow();

    // Nothing wrong, return.
    return 0;

}

void EditorNew::Draw()
{

}

void EditorNew::DrawUI()
{

}

void EditorNew::Update()
{
    
    // Safety, can't use the editor here.
    if (!enabled)
    {
        FadeEffect();
        return;
    }

}

void EditorNew::FadeEffect()
{
    Vector3 col = ColorToHSV(fadeColor);
    col.x += 0.25;
    if (col.x >= 360)
    {
        col.x = 0;
    }
    fadeColor = ColorFromHSV(col.x, col.y, col.z);
}