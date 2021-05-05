#include <map>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui/rlImGui.h"
#include "px/tileset.h"
#include "px/pxmap.h"

const float SCROLL_SPEED = 5.0;
const float SCROLL_SPEED_DIV = 1.0;

int main(int argc, char *argv[])
{
    int screenWidth = 1240;
    int screenHeight = 720;
    Camera2D cam;
    cam.offset = { 0.0, 0.0 };
    cam.rotation = 0.0;
    cam.target = { 0, 0 };
    cam.zoom = 1.0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Kero Master");

    SetTargetFPS(60);
    SetupRLImGui(true);

    map<string, Tileset> tilesets;
    Map m;
    m.Load("Pink Hour/rsc_p", "01PHlastE", tilesets);
    Tileset t;
    t.Load("Pink Hour/rsc_p", "fuFixNPC");
    tilesets["fuFixNPC"] = t;
    t.Load("Pink Hour/rsc_p", "fuFixChar");
    tilesets["fuFixChar"] = t;
    t.Load("Pink Hour/rsc_p", "fuFixPtcle");
    tilesets["fuFixPtcle"] = t;
    Vector2 off = { 0, 0 };
    Vector2 scrollOff = { 0, 0 };
    f32 scale = 3.0f;
    map<u8, EntityDisplay> entities = LoadXML("all");
    bool debug = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        m.Clear();
        BeginMode2D(cam);
        m.DrawLayer(2, tilesets, scrollOff, scale, debug);
        m.DrawLayer(1, tilesets, off, scale, debug);
        m.DrawLayer(0, tilesets, off, scale, debug);
        m.DrawEntities(entities, tilesets, off, scale, true);
        EndMode2D();
        BeginRLImGui();
        ImGui::ShowDemoWindow();
        EndRLImGui();
        EndDrawing();
        if (IsKeyDown(KEY_W))
        {
            off.y += SCROLL_SPEED;
            scrollOff.y += SCROLL_SPEED;
        }
        if (IsKeyDown(KEY_S))
        {
            off.y -= SCROLL_SPEED;
            scrollOff.y -= SCROLL_SPEED;
        }
        if (IsKeyDown(KEY_A))
        {
            off.x += SCROLL_SPEED;
            scrollOff.x += SCROLL_SPEED / SCROLL_SPEED_DIV;
        }
        if (IsKeyDown(KEY_D))
        {
            off.x -= SCROLL_SPEED;
            scrollOff.x -= SCROLL_SPEED / SCROLL_SPEED_DIV;
        }
        float scaleAdjust = GetMouseWheelMove();
        if (scaleAdjust != 0)
        {
            //scale += scaleAdjust;
            cam.zoom += scaleAdjust * 0.5f;
            /*if (scaleAdjust > 0)
            {
                off.x *= scaleAdjust;
                off.y *= scaleAdjust;
                scrollOff.x *= scaleAdjust;
                scrollOff.y *= scaleAdjust;
            }
            else
            {
                off.x /= -scaleAdjust;
                off.y /= -scaleAdjust;
                scrollOff.x /= -scaleAdjust;
                scrollOff.y /= -scaleAdjust;
            }*/
        }
    }
    ShutdownRLImGui();
    CloseWindow();

    return 0;
}
