#pragma once

#include <fstream>
#include <string>
#include "../types.h"

struct Editor;
struct Entity;

// For editing the currently selected entity.
struct EntityEditor
{
    Editor* ed; // Original editor instance.
    Entity* editingEntity = nullptr; // Entity to currently edit.
    char** entityListing = nullptr; // List of entities to display.
    int numEntities = 0; // Size of entity listing.

    // Create a new entity editor.
    EntityEditor(Editor* ed);

    // Load the entity listing.
    void LoadEntityListing();

    // Draw the interface for the editor.
    void DrawUI();

    // Edit a flag.
    void EditFlagBit(u8& dat, u8 bit, std::string name, std::string desc, int itemWidth);

};