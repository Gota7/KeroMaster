#pragma once

#include <fstream>
#include <map>
#include <string>
#include "../types.h"

struct Editor;
struct Entity;
struct EntityDisplay;

// For editing the currently selected entity.
struct EntityEditor
{
    Editor* ed; // Original editor instance.
    std::map<u8, EntityDisplay> entities; // Loaded entities.
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