#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include <map>
#include "str.h"
#include "gbin/gtypes.h"
#include "tileset.h"
#include "../ed/entityDisplay.h"

using namespace std;

const int NUM_REFERENCES = 5;
const int NUM_SETTINGS = 8;
const int NUM_TILESETS = 3;
const int NUM_PARAMETERS = 3;
const int NUM_BYTE_PARAMETERS = 2;

struct PxMap : GReadable, GWriteable
{
    u16 width;
    u16 height;
    u8 flags;
    u8* tiles;
    
    void Read(GFile* f);
    void Write(GFile* f);
    u8 GetTile(u8 index);
    u8 GetTile(u8 x, u8 y);
    void Unload();
};

enum ReferenceType
{
    RT_SCRIPT,
    RT_NEXT_LEVEL,
    RT_PREV_LEVEL,
    RT_LINK_LEVEL,
    RT_NPC_PALETTE
};

struct Entity
{
    u8 flags;
    u8 id;
    u8 unk;
    u16 xPos;
    u16 yPos;
    u8 parametersByte[NUM_BYTE_PARAMETERS];
    Str parametersStr[NUM_PARAMETERS - NUM_BYTE_PARAMETERS];
};

struct Map
{
    Str comment;
    Str references[NUM_REFERENCES];
    u8 levelSettings[NUM_SETTINGS];     // u16; u16; u8; u8 BG Red; u8 BG Green; u8 BG Blue.
    Str tilesets[NUM_TILESETS];
    u8 tilesetSettings1[NUM_TILESETS];
    u8 tilesetSettings2[NUM_TILESETS];
    PxMap maps[NUM_TILESETS];
    vector<Entity> entities;

    void Load(string rsc_k, string mapName, map<string, Tileset>& tilesets);
    void Unload(map<string, Tileset>& tilesets);
    void Clear();
    void DrawLayer(u8 layerNum, map<string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool showAttr);
    void DrawEntities(map<u8, EntityDisplay>& entities, map<string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool debug = false);
};