#include <map>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui/rlImGui.h"
#include "px/tileset.h"
#include "px/pxmap.h"
#include "ed/editor.h"

const float SCROLL_SPEED = 5.0;
const float SCROLL_SPEED_DIV = 1.0;
const int SCREEN_WIDTH = 1240;
const int SCREEN_HEIGHT = 720;

int main(int argc, char *argv[])
{

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kero Master");

    SetTargetFPS(60);
    SetupRLImGui(true);

    Editor e;
    e.SetPath("Kero Blaster/rsc_k");
    e.LoadEnemies("all");
    e.LoadFixedTilesets();
    e.LoadLevel("01field2");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        e.Draw();
        BeginRLImGui();
        e.DrawUI();
        ImGui::ShowDemoWindow();
        EndRLImGui();
        EndDrawing();
        e.Update();
    }
    ShutdownRLImGui();
    CloseWindow();

    return 0;
}
