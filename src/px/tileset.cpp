#include "tileset.h"
#include <stdio.h>
#include <stdlib.h>

Texture2D Tileset::attrTex;

void Tileset::Load(string rsc_k, string tilesetName)
{
    if (GFile::FileExists((rsc_k + "/img/" + tilesetName + ".png").c_str()))
    {
        tex = LoadTexture((rsc_k + "/img/" + tilesetName + ".png").c_str());
    }
    else
    {
        tex = LoadTexture((rsc_k + "/localize/en.lproj/" + tilesetName + ".png").c_str());
    }
    width = oldWidth = 16;
    height = oldHeight = 16;
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
            if (width * height > 0) {
                flags = f.ReadU8();
                tiles = new u8[width * height];
                for (int i = 0; i < width * height; i++) {
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

u8 Tileset::GetTilesetAttr(u8 index)
{
    if (tiles == NULL)
    {
        return 0;
    }
    else {
        return tiles[index];
    }
}

u8 Tileset::GetTilesetAttr(u8 x, u8 y)
{
    return GetTilesetAttr(x + y * width);
}

void Tileset::Draw(u8 index, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool allowDrawIndex0, bool overrideWidth, s8 xOffPixels, s8 yOffPixels)
{
    if ((index == 0 && !allowDrawIndex0) || width == 0)
    {
        return;
    }
    Vector2 off = { origin.x + xOff * 8 * mapScale + xOffPixels, origin.y + yOff * 8 * mapScale + yOffPixels };
    u16 w = overrideWidth ? (tex.width / 8) : width;
    DrawTextureTiled(tex, { (float)(index % w * 8), (float)(index / w * 8), 8, 8 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale, WHITE);
    if (showAttr && tiles != NULL)
    {
        u8 attr = GetTilesetAttr(index);
        if (attr != 0)
        {
            //DrawText(to_string(attr).c_str(), (int)(off.x + 1 * mapScale), (int)(off.y + 1 * mapScale), (int)(5 * mapScale), YELLOW);
            DrawTextureTiled(attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, 16 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale / 2, WHITE);
        }
    }
}

void Tileset::Draw(u16 x, u16 y, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool showAttr, bool overrideWidth, s8 xOffPixels, s8 yOffPixels)
{
    Vector2 off = { origin.x + xOff * 8 * mapScale + xOffPixels * mapScale, origin.y + yOff * 8 * mapScale + yOffPixels * mapScale };
    u16 w = overrideWidth ? (tex.width / 8) : width;
    DrawTextureTiled(tex, { (float)x * 8, (float)y * 8, 8, 8 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale, WHITE);
    if (showAttr && tiles != NULL)
    {
        u8 attr = GetTilesetAttr(x, y);
        if (attr != 0)
        {
            //DrawText(to_string(attr).c_str(), (int)(off.x + 1 * mapScale), (int)(off.y + 1 * mapScale), (int)(5 * mapScale), YELLOW);
            DrawTextureTiled(attrTex, { (float)(attr % 16 * 16), (float)(attr / 16 * 16), 16, 16 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale / 2, WHITE);
        }
    }
}