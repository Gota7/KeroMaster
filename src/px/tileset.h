#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include "gbin/gfile.h"

using namespace std;

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
    constexpr static float ATTR_TILE_SIZE = 16;

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
    Rectangle GetSrcRect(u8 index, f32 tileSize, u32 tilesPerRow);

    // Get the destination rectangle of where to draw a tile.
    Rectangle GetDestRect(s32 tileX, s32 tileY, f32 tileSize, Vector2 offset = { 0, 0 });

    // Draw a tile attribute.
    void DrawTileAttr(u8 attr, s32 tileX, s32 tileY, f32 destTileSize, Color tint = WHITE);

    // Draw a tile index to a destination tile position with a destination tile size.
    void Draw(u8 index, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr = false, bool allowIndex0 = false, bool useTrueImageSize = false, Vector2 offset = { 0, 0 }, Color tint = WHITE);

    // Draw a tile from x and y to a destination tile position with a destination tile size.
    void Draw(u8 xIndex, u8 yIndex, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr = false, bool allowIndex0 = false, bool useTrueImageSize = false, Vector2 offset = { 0, 0 }, Color tint = WHITE);

    // THIS CODE BELOW IS OBSOLETE AND WILL BE DELETED!!!

    void Draw(u8 index, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool allowDrawIndex0 = false, bool overrideWidth = false, s8 xOffPixels = 0, s8 yOffPixels = 0, Color tint = WHITE, u8 textureScale = 2);
    void Draw(u16 x, u16 y, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool overrideWidth = false, s8 xOffPixels = 0, s8 yOffPixels = 0, Color tint = WHITE, u8 textureScale = 2);

};