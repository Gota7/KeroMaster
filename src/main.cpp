#include <map>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui/rlImGui.h"
#include "px/tileset.h"
#include "px/pxmap.h"
#include "ed/editor.h"

constexpr int SCREEN_WIDTH = 1240;
constexpr int SCREEN_HEIGHT = 720;

static int EditorLoop() {
    Editor e;

    try {
        e.SetPath("Kero Blaster/rsc_k");
        e.LoadEnemies("all");
        e.LoadFixedTilesets();
        e.LoadLevel("01field1");
        Tileset t;
        t.Load(e.rsc, "mpt01");
        t.Write(e.rsc, "DUMMY");
    } catch (string &e) {
        TraceLog(LOG_ERROR, "Failed to load game data: %s\n", e.c_str());
        return 1;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        e.Draw();
        BeginRLImGui();
        e.DrawUI();
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
