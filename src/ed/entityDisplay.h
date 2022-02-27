#pragma once

#include <string>
#include <cstring>
#include <map>
#include <stdexcept>
#include "../tinyxml2/tinyxml2.h"
#include "../px/tileset.h"
#include "../px/str.h"
#include "../types.h"

using namespace std;
using namespace tinyxml2;

// A single tile for an entity to display.
struct EntityTile
{
    u16 x = 0; // X tile.
    u16 y = 0; // Y tile.
    s32 xOff = 0; // Source x offset in pixels.
    s32 yOff = 0; // Source y offset in pixels.
    u16 numTilesX = 1; // Width in tiles.
    u16 numTilesY = 1; // Height in tiles.
    s8 xOffPixels = 0; // Destination x offset in pixels.
    s8 yOffPixels = 0; // Destination y offset in pixels.
    string tileset = ""; // Tileset to use. It can be a fixed one, blank for the NPC sheet, or /1, /2, or /3 for corresponding map tilesets.
    u8 flagMode = true; // If this tile only appears if a bit in flags is set.
    u8 flagBit = 0; // The bit in flags to be set to show this tile if in flag mode.
};

// Entity display information.
struct EntityDisplay
{
    string name = ""; // Entity name.
    string description = ""; // What the entity does.
    string flagDescription = ""; // How the flag works.
    string parameterDescriptions[3] = { "", "", "" }; // How the parameters work.
    int numTiles = 0; // Number of entity tiles.
    EntityTile* tiles; // Entity tiles to display.
    bool allowRollYourOwnSprite = true; // Allow for custom sprites to show specified by a parameter.
    static map<string, EntityTile> rollYourOwnSprite; // Maps strings to sprites that can be shown in game.
    static float transparency; // Transparency of the entity to display.

    // Draw a box with the entity ID.
    void DrawEntityIdBox(u8 id, s32 tileX, s32 tileY, bool beingEdited, Vector2 offset = { 0, 0 });

    void Draw(u8 id, Str strParam, u8 flags, bool beingEdited, string spritesheetName, string tilesetNames[3], map<string, Tileset>& tilesets, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool debug = false);
};

// Load an XML map for all entities.
map<u8, EntityDisplay> LoadXML(string game);