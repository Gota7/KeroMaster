#pragma once

#include <string>
#include <cstring>
#include <map>
#include <stdexcept>
#include <vector>
#include "raylib.h"
#include "../px/str.h"
#include "../types.h"

struct Tileset;

// Entity data constants.
const int NUM_PARAMETERS = 3;
const int NUM_BYTE_PARAMETERS = 2;

// Information on how to spawn entities.
struct Entity
{
    u8 flags; // Entity flags.
    u8 id; // Id to spawn.
    u8 unk; // Unknown value.
    u16 xPos; // X position.
    u16 yPos; // Y position.
    u8 parametersByte[NUM_BYTE_PARAMETERS]; // Number parameters.
    Str parametersStr[NUM_PARAMETERS - NUM_BYTE_PARAMETERS]; // String parameters.
    bool beingEdited = false; // Editor param.
};

// A single tile for an entity to display.
struct EntityTile
{
    u16 x = 0; // X tile.
    u16 y = 0; // Y tile.
    s32 xOff = 0; // Destination x offset in tiles.
    s32 yOff = 0; // Destination x offset in tiles.
    u16 numTilesX = 1; // Width in tiles.
    u16 numTilesY = 1; // Height in tiles.
    s8 xOffPixels = 0; // Destination x offset in pixels.
    s8 yOffPixels = 0; // Destination y offset in pixels.
    std::string tileset = ""; // Tileset to use. It can be a fixed one, blank for the NPC sheet, or /0, /1, or /2 for corresponding map tilesets.
    u8 flagMode = true; // If this tile only appears if a bit in flags is set.
    u8 flagBit = 0; // The bit in flags to be set to show this tile if in flag mode. 1 is added, as 0 means disabled.
    u16 unkDraw = 0; // Draw using the unknown parameter. 1 is added to this as 0 means disabled.
};

// Entity display information.
struct EntityDisplay
{
    std::string name = ""; // Entity name.
    std::string description = ""; // What the entity does.
    int numTiles = 0; // Number of entity tiles.
    EntityTile* tiles; // Entity tiles to display.
    bool allowRollYourOwnSprite = true; // Allow for custom sprites to show specified by a parameter.
    static std::string parameterNames[3]; // Names of each parameter.
    static std::string parameterDescriptions[3]; // How the parameters work.
    static std::map<std::string, std::vector<EntityTile>> rollYourOwnSprite; // Maps strings to sprites that can be shown in game.
    static float transparency; // Transparency of the entity to display.

    // Draw a box with the entity ID.
    void DrawEntityIdBox(u8 id, s32 tileX, s32 tileY, bool beingEdited, Vector2 offset = { 0, 0 });

    // Draw unit type image for an entity.
    void DrawEntityUnitType(u8 id, s32 tileX, s32 tileY, Vector2 offset = { 0, 0 });

    // Draw a bunch of entity tiles.
    void DrawEntityTiles(Tileset& ts, u16 x, u16 y, u16 width, u16 height, s32 tileX, s32 tileY, Vector2 offset = { 0, 0 });

    // Draw an entity tile.
    void DrawEntityTile(EntityTile* tile, std::map<std::string, Tileset>& loadedTilesets, std::string spriteSheet, std::string tilesetNames[3], s32 tileX, s32 tileY, Vector2 offset = { 0, 0 });

    // Draw an entity.
    void Draw(Entity* entity, std::map<std::string, Tileset>& loadedTilesets, std::string spriteSheet, std::string tilesetNames[3], bool debug = false, Vector2 offset = { 0, 0 });

};

// Load an XML map for all entities.
std::map<u8, EntityDisplay> LoadXML(std::string game);