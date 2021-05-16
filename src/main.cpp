#include <map>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui/rlImGui.h"
#include "px/tileset.h"
#include "px/pxmap.h"
#include "ed/editor.h"

constexpr int SCREEN_WIDTH = 1240;
constexpr int SCREEN_HEIGHT = 720;
bool runOnce = true;

static int EditorLoop() {
    Editor e;

    try {
        e.settings.Load();
        if (strcmp(e.settings.rscPath.c_str(), "") == 0 || strcmp(e.settings.lastLevel.c_str(), "") == 0)
        {
            e.settings.show = true;
        }
        else
        {
            if (GFile::FileExists((e.settings.rscPath + "/field/" + e.settings.lastLevel + ".pxpack").c_str()))
            {
                e.SetPath(e.settings.rscPath);
                e.LoadLevel(e.settings.lastLevel);
            }
            else
            {
                e.settings.show = true;
            }
        }
        e.LoadEnemies("all");
        e.LoadFixedTilesets();  
    } catch (string &e) {
        TraceLog(LOG_ERROR, "Failed to load game data: %s\n", e.c_str());
        return 1;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if (e.settings.show)
        {
            ClearBackground(BLACK);
            BeginRLImGui();
            if (runOnce)
            {
                ImGui::OpenPopup("Editor Settings");
            }
            e.settings.ShowWindow(&e);
        }
        else
        {
            e.Draw();
            BeginRLImGui();
            e.DrawUI();
        }
        //ImGui::ShowDemoWindow();
        EndRLImGui();
        EndDrawing();
        e.Update();
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int exit = 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kero Master");

    SetTargetFPS(60);
    SetupRLImGui(true);
    exit = EditorLoop();
    ShutdownRLImGui();
    CloseWindow();

    return exit;
}
