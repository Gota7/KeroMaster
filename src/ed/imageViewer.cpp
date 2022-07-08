#include "imageViewer.h"

#include "editor.h"
#include "../rlImGui/rlImGui.h"

ImageViewer::ImageViewer(Editor* ed, std::string path)
{
    open = true;
    this->ed = ed;
    this->path = path;
    name = GetFileName(path.c_str());
    for (auto& viewer : ed->imageViewers)
    {
        if (viewer.path == path)
        {
            open = false; // No duplicate images open.
        }
    }
    tex = LoadTexture(path.c_str());
}

void ImageViewer::DrawUI()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Determine which layers are allowed and selected.
    ImGui::Begin(("Image - " + name).c_str(), &open, ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();

    // Scale the tileset image properly.
    float w = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();
    float h = ImGui::GetWindowHeight() - ImGui::GetCursorPosY();
    float texW = tex.width;
    float texH = tex.height;
    if (w / texW > h / texH)
    {
        float scale = h / texH;
        w = texW * scale;
        h = texH * scale;
    }
    else
    {
        float scale = w / texW;
        w = texW * scale;
        h = texH * scale;
    }

    // Set variables, draw the image.
    imgPos = ImGui::GetCursorScreenPos();
    imgSizeX = w;
    imgSizeY = h;
    RLImGuiImageSize(&tex, (int)h, (int)w);
    ImGui::End();

}

void ImageViewer::Close()
{
    open = false;
    UnloadTexture(tex);
}