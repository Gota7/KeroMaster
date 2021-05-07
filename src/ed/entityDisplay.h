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

struct EntityTile
{
    u16 x = 0;
    u16 y = 0;
    s32 xOff = 0;
    s32 yOff = 0;
    u16 numTilesX = 1;
    u16 numTilesY = 1;
    s8 xOffPixels = 0;
    s8 yOffPixels = 0;
    string tileset = "";
    u8 flagMode = true;
    u8 flagBit = 0;
};

struct EntityDisplay
{
    string name = "";
    string description = "";
    string flagDescription = "";
    string parameterDescriptions[3];
    int numTiles = 0;
    EntityTile* tiles;
    bool allowRollYourOwnSprite = true;
    static map<string, EntityTile> rollYourOwnSprite;
    static float transparency;

    void Draw(u8 id, Str strParam, u8 flags, bool beingEdited, string spritesheetName, string tilesetNames[3], map<string, Tileset>& tilesets, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool debug = false);
};

map<u8, EntityDisplay> LoadXML(string game);