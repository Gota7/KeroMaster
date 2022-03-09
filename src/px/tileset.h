#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include "gbin/gfile.h"

using namespace std;

// Tilesets used for levels.
struct Tileset
{
    Texture2D tex; // Loaded internal texture.
    u8* tiles = NULL; // Attribute data array.
    u8 flags; // Tileset flags.
    u16 width; // Tileset width in tiles.
    u16 height; // Tileset height in tiles.
    static Texture2D attrTex; // Textures to use for the tileset attributes.
    static Texture2D unitType; // Unit type texture.
    u16 oldWidth; // Editor param.
    u16 oldHeight; // Editor param.

    // All tiles will be displayed as this in game.
    constexpr static float MAP_TILE_SIZE = 8;
    constexpr static float ENTITY_TILE_SIZE = 4;
    constexpr static float EDITOR_TILE_SIZE = 16;
    constexpr static float ATTR_TILE_SIZE = 16;
    constexpr static float UNIT_TYPE_TILE_SIZE = 16;

    // Load a tileset from the tileset section of the resource folder.
    void Load(string rsc_k, string tilesetName);

    // Unload the current tileset.
    void Unload();

    // Save this tileset to the tileset section of the resource folder.
    void Write(string rsc_k, string tilesetName);

    // Get tileset attributes at a tile index.
    u8 GetTilesetAttr(u8 index);

    // Get tileset attributes from a position in the tileset.
    u8 GetTilesetAttr(u8 x, u8 y);

    // Get the name of a tileset attribute at an index.
    const char* GetAttributeName(u8 index);

    // Get how many tiles are in each row.
    u16 TilesPerRow(u32 tileSize, bool useTrueImageSize);

    // Get the source rectangle of a tile to draw. There is also an option to use the true image size for determining tiles per row.
    static Rectangle GetSrcRect(u8 index, f32 tileSize, u32 tilesPerRow);

    // Get the source rectangle of a tile to draw. Dummy is for overloading convenience.
    static Rectangle GetSrcRect(u16 xIndex, u16 yIndex, f32 tileSize, bool dummy);

    // Get the destination rectangle of where to draw a tile.
    static Rectangle GetDestRect(s32 tileX, s32 tileY, f32 tileSize, Vector2 offset = { 0, 0 });

    // Draw a tile attribute.
    static void DrawTileAttr(u8 attr, s32 tileX, s32 tileY, f32 destTileSize, Color tint = WHITE);

    // Draw a tile index to a destination tile position with a destination tile size.
    void Draw(u8 index, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr = false, bool allowIndex0 = false, bool useTrueImageSize = false, Vector2 offset = { 0, 0 }, Color tint = WHITE);

    // Draw a tile from x and y to a destination tile position with a destination tile size.
    void Draw(u8 xIndex, u8 yIndex, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr = false, bool allowIndex0 = true, bool useTrueImageSize = false, Vector2 offset = { 0, 0 }, Color tint = WHITE);

};