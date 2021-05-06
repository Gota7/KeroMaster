#include "tilesetEditor.h"

TilesetEditor::TilesetEditor(Editor* ed, string name)
{
    open = true;
    this->ed = ed;
    this->name = name;
}

void TilesetEditor::Draw()
{

    // Safety.
    if (!open)
    {
        return;
    }

}

void TilesetEditor::DrawUI()
{
    
    // Safety.
    if (!open)
    {
        return;
    }

    // Interface.
    ImGui::Begin(("Tileset - " + name).c_str(), &open);
    ed->focus.ObserveFocus();
    ImGui::Button("Hi!");
    ImGui::End();
    
}

void TilesetEditor::Update()
{

    // Safety.
    if (!open)
    {
        return;
    }

}

void TilesetEditor::Close()
{
    open = false;
}