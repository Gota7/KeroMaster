#include "tileset.h"
#include <stdio.h>
#include <stdlib.h>

Texture2D Tileset::attrTex;
Texture2D Tileset::unitType;

void Tileset::Load(string rsc_k, string tilesetName)
{

    // Where to search for tilesets.
    static std::vector<string> lookupPaths = {
        "/img/",
        "/localize/en.lproj/",
        "/localize/ja.lproj/",
    };

    // Load the tileset.
    for (auto& path : lookupPaths) {
        string imagePath = rsc_k + path + tilesetName + ".png";

        if (GFile::FileExists(imagePath.c_str()))
        {
            Image image = LoadImage(imagePath.c_str());

            if (image.data != nullptr)
            {
                tex = LoadTextureFromImage(image);
                UnloadImage(image);
                break;
            }
        }
    }

    // The default width and heights are 16 for some reason.
    width = oldWidth = 16;
    height = oldHeight = 16;

    // Read tileset attributes if they exist.
    if (GFile::FileExists((rsc_k + "/img/" + tilesetName + ".pxattr").c_str()))
    {
        GFile f = GFile((rsc_k + "/img/" + tilesetName + ".pxattr").c_str());
        if (strcmp(f.ReadStr().c_str(), "pxMAP01"))
        {
            printf("Invalid tileset attribute magic!\n");
            throw new exception();
        }
        else
        {
            width = oldWidth = f.ReadU16();
            height = oldHeight = f.ReadU16();
            if (width * height > 0)
            {
                flags = f.ReadU8();
                tiles = new u8[width * height];
                for (int i = 0; i < width * height; i++)
                {
                    tiles[i] = f.ReadU8();
                }
            }
        }
        f.Close();
    }
    
}

void Tileset::Unload()
{
    UnloadTexture(tex);
    if (tiles != NULL) {
        delete[] tiles;
        tiles = NULL;
    }
}

void Tileset::Write(string rsc_k, string tilesetName)
{
    GFile f = GFile((rsc_k + "/img/" + tilesetName + ".pxattr").c_str());
    f.WriteNullTerminated("pxMAP01");
    f.Write(tiles != NULL ? width : (u16)0);
    f.Write(tiles != NULL ? height : (u16)0);
    if (width * height > 0 && tiles != NULL)
    {
        f.Write(flags);
        for (int i = 0; i < width * height; i++)
        {
            f.Write(tiles[i]);
        }
    }
}

const char* Tileset::GetAttributeName(u8 index) {
    u8 attr = GetTilesetAttr(index);

    switch (attr) {
        case 0: return "non-solid";
        case 1: return "solid";
        case 2: return "one-way from right??";
        case 3: return "one-way from left??";
        case 4: return "solid breakable";
        case 5: return "spike";
        case 7: return "platform";
        case 8: return "top left high 22.5° slope";
        case 9: return "top left low 22.5° slope";
        case 10: return "top right low 22.5° slope";
        case 11: return "top right high 22.5° slope";
        case 12: return "bottom left high 22.5° slope";
        case 13: return "bottom left low 22.5° slope";
        case 14: return "bottom right low 22.5° slope";
        case 15: return "bottom right high 22.5° slope";
        case 33: return "moving platform marker left";
        case 34: return "moving platform marker right";
        case 35: return "moving platform marker up";
        case 36: return "moving platform marker down";
        case 40: return "top left 45° slope";
        case 41: return "top right 45° slope";
        case 42: return "bottom left 45° slope";
        case 43: return "bottom right 45° slope";
        case 49: return "ladder";
        case 50: return "ladder top";
        case 51: return "player block/npc non-solid";
        case 52: return "player non-solid/npc block?";
        case 56: return "mud";
        case 57: return "ice";
        case 58: return "snow";
        case 64: return "non-solid water";
        case 65: return "solid water";
        default:
            return "unknown";
    }
}

u8 Tileset::GetTilesetAttr(u8 index)
{
    if (tiles == NULL)
    {
        return 0;
    }
    else
    {
        return tiles[index];
    }
}

u8 Tileset::GetTilesetAttr(u8 x, u8 y)
{
    return GetTilesetAttr(x + y * width);
}

u16 Tileset::TilesPerRow(u32 tileSize, bool useTrueImageSize)
{
    return useTrueImageSize ? (u16)(tex.width / tileSize) : width;
}

Rectangle Tileset::GetSrcRect(u8 index, f32 tileSize, u32 tilesPerRow)
{
    return
    {
        index % tilesPerRow * tileSize,
        index / tilesPerRow * tileSize,
        tileSize,
        tileSize
    };
}

Rectangle Tileset::GetDestRect(s32 tileX, s32 tileY, f32 tileSize, Vector2 offset)
{
    return
    {
        (float)tileX * tileSize + offset.x,
        (float)tileY * tileSize + offset.y,
        tileSize,
        tileSize
    };
}

void Tileset::DrawTileAttr(u8 attr, s32 tileX, s32 tileY, f32 destTileSize, Color tint)
{
    if (attr != 0)
    {
        DrawTexturePro(
            attrTex,
            GetSrcRect(attr, ATTR_TILE_SIZE, attrTex.width / ATTR_TILE_SIZE),
            GetDestRect(tileX, tileY, destTileSize),
            { 0, 0 },
            0,
            tint
        );
    }
}

void Tileset::Draw(u8 index, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr, bool allowIndex0, bool useTrueImageSize, Vector2 offset, Color tint)
{

    // Check for valid index.
    if ((index == 0 && !allowIndex0) || width == 0)
    {
        return;
    }

    // Draw the actual texture.
    DrawTexturePro(
        tex,
        GetSrcRect(index, tileSize, TilesPerRow(tileSize, useTrueImageSize)),
        GetDestRect(tileX, tileY, destTileSize, offset),
        { 0, 0 },
        0,
        tint
    );

    // Attribute drawing.
    if (showAttr) DrawTileAttr(GetTilesetAttr(index), tileX, tileY, destTileSize, tint);

}

void Tileset::Draw(u8 xIndex, u8 yIndex, s32 tileX, s32 tileY, f32 tileSize, f32 destTileSize, bool showAttr, bool allowIndex0, bool useTrueImageSize, Vector2 offset, Color tint)
{
    u8 index = xIndex * TilesPerRow(tileSize, useTrueImageSize) + yIndex;
    Draw(index, tileX, tileY, tileSize, destTileSize, showAttr, allowIndex0, useTrueImageSize, offset, tint);
}

// THIS CODE BELOW IS OBSOLETE AND WILL BE DELETED!!!

void Tileset::Draw(u8 index, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool allowDrawIndex0, bool overrideWidth, s8 xOffPixels, s8 yOffPixels, Color tint, u8 textureScale)
{
    if ((index == 0 && !allowDrawIndex0) || width == 0)
    {
        return;
    }

    const float tileWidth = (float) (16 / textureScale);

    Vector2 off = { origin.x + xOff * 8 * mapScale + xOffPixels, origin.y + yOff * 8 * mapScale + yOffPixels };
    u16 w = overrideWidth ? (tex.width / 8) : width;
    
    const Rectangle src = { (float)(index % w * tileWidth), (float)(index / w * tileWidth), tileWidth, tileWidth };
    const Rectangle dst = { off.x, off.y, 8 * mapScale, 8 * mapScale };

    DrawTextureTiled(tex, src, dst, { 0, 0 }, 0, mapScale / (tileWidth / 8.0f), tint);
    if (showAttr && tiles != NULL)
    {
        u8 attr = GetTilesetAttr(index);
        if (attr != 0)
        {
            //DrawText(to_string(attr).c_str(), (int)(off.x + 1 * mapScale), (int)(off.y + 1 * mapScale), (int)(5 * mapScale), YELLOW);
            //DrawTextureTiled(attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, 16 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale / 2, tint);
            DrawTileAttr(attr, xOff, yOff, 8);
        }
    }
}

void Tileset::Draw(u16 x, u16 y, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool overrideWidth, s8 xOffPixels, s8 yOffPixels, Color tint, u8 textureScale)
{
    if (textureScale == 0) textureScale = 2;
    const float tileWidth = (float) (16 / textureScale);

    Vector2 off = { origin.x + xOff * 8 * mapScale + xOffPixels * mapScale, origin.y + yOff * 8 * mapScale + yOffPixels * mapScale };
    u16 w = overrideWidth ? (tex.width / 8) : width;
    DrawTextureTiled(tex, { (float)x * tileWidth, (float)y * tileWidth, tileWidth, tileWidth }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale, tint);

    if (showAttr && tiles != NULL)
    {
        u8 attr = GetTilesetAttr(x, y);
        if (attr != 0)
        {
            //DrawText(to_string(attr).c_str(), (int)(off.x + 1 * mapScale), (int)(off.y + 1 * mapScale), (int)(5 * mapScale), YELLOW);
            //DrawTextureTiled(attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, 16 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale / 2, tint);
            DrawTileAttr(attr, xOff, yOff, 8);
        }
    }
}