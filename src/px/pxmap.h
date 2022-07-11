#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include <map>
#include "str.h"
#include "gbin/gtypes.h"

// Forward declarations.
struct Entity;
struct EntityDisplay;
struct Tileset;

// Level data constants.
const int NUM_REFERENCES = 5;
const int NUM_SETTINGS = 8;
const int NUM_TILESETS = 3;

// How to shift blocks in a map. A resize shift direction will resize the map instead.
enum class ShiftDirection
{
    Up,
    Down,
    Left,
    Right,
    Resize
};

// Map layers.
enum class MapLayer
{
    FG,
    MG,
    BG
};

// Contains raw tile data.
struct PxMap : GReadable, GWriteable
{
    u16 width = 0; // Width of the map in tiles.
    u16 height = 0; // Height of the map in tiles.
    u8 flags = 0; // Map flags.
    u8* tiles = nullptr; // Map data.
    u16 newWidth = 0; // For editor.
    u16 newHeight = 0; // For editor.
    static const char* layerNames[]; // Map layer names.

    // Read a map file.
    void Read(GFile* f);

    // Write a map file.
    void Write(GFile* f);

    // Set a new tile at an index, return the old tile ID.
    u8 SetTile(u32 index, u8 newTile);

    // Set a new tile at a position, return the old tile ID.
    u8 SetTile(u16 x, u16 y, u8 newTile);

    // Get a tile ID at an index.
    u8 GetTile(u32 index);

    // Get a tile ID at a position.
    u8 GetTile(u16 x, u16 y);

    // Resize the map.
    void Resize(u16 newWidth, u16 newHeight);

    // If it is possible to shift all the blocks in a particular direction.
    bool CanShift(ShiftDirection dir);

    // Shift all the blocks in a particular direction.
    void Shift(ShiftDirection dir);

    // Unload all the map data.
    void Unload();

};

// Reference data stored.
enum ReferenceType
{
    RT_LEFT_LEVEL,
    RT_RIGHT_LEVEL,
    RT_UP_LEVEL,
    RT_DOWN_LEVEL,
    RT_NPC_PALETTE
};

// PxPack which is a level that contains various map data.
struct Map
{
    Str levelTitle; // Name of the level.
    Str references[NUM_REFERENCES]; // See the references enum.
    u8 levelSettings[NUM_SETTINGS]; // u16 Area X; u16 Area Y; u8 Area Number; u8 BG Red; u8 BG Green; u8 BG Blue.
    Str tilesets[NUM_TILESETS]; // Tileset names.
    u8 tilesetSettings1[NUM_TILESETS]; // 16 / NUM = Tile size.
    u8 tilesetSettings2[NUM_TILESETS]; // Scroll mode.
    PxMap maps[NUM_TILESETS]; // Map layers for the level.
    std::vector<Entity> entities; // Entity placements.

    // Load a level from the resource folder. Requires a reference to loaded tilesets.
    void Load(std::string rsc_k, std::string mapName, std::map<std::string, Tileset>& tilesets);

    // Unload a level from the resource folder. Requires a reference to loaded tilesets.
    void Unload(std::map<std::string, Tileset>& tilesets);

    // Write the map data.
    void Write(std::string rsc_k, std::string mapName);

    // Clear the screen with the map's background color.
    void Clear();

    // Get the tile size.
    float TileSize(int layerNum);

    // Draw a map layer using the loaded tilesets.
    void DrawLayer(u8 layerNum, std::map<std::string, Tileset>& tilesets, Vector2 origin, bool showAttr);

    // Draw the entities on the map using the loaded tilesets.
    void DrawEntities(std::map<u8, EntityDisplay>& entities, std::map<std::string, Tileset>& tilesets, Vector2 origin, bool debug = false);

    // Import a tile layer from a file.
    void ImportLayer(std::string path, int layerNum);

    // Export a tile layer to a file.
    void ExportLayer(std::string path, int layerNum);

    // Import entities from a file.
    void ImportEntities(std::string path);

    // Export entities to a file.
    void ExportEntities(std::string path);

};