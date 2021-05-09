#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include <map>
#include "str.h"
#include "gbin/gtypes.h"
#include "tileset.h"
#include "../ed/entityDisplay.h"
#include "../conv/conv.h"

const int NUM_REFERENCES = 5;
const int NUM_SETTINGS = 8;
const int NUM_TILESETS = 3;
const int NUM_PARAMETERS = 3;
const int NUM_BYTE_PARAMETERS = 2;

enum ShiftDirection
{
    SHIFT_UP,
    SHIFT_DOWN,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    SHIFT_RESIZE
};

struct PxMap : GReadable, GWriteable
{
    u16 width = 0;
    u16 height = 0;
    u8 flags = 0;
    u8* tiles = nullptr;
    u16 oldWidth = 0; // For editor.
    u16 oldHeight = 0; // For editor.
    
    void Read(GFile* f);
    void Write(GFile* f);
    u8 GetTile(u8 index);
    u8 GetTile(u8 x, u8 y);
    void Resize(u16 newWidth, u16 newHeight);
    bool CanShift(ShiftDirection dir);
    void Shift(ShiftDirection dir);
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
    bool beingEdited = false; // Editor param.
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
    std::vector<Entity> entities;

    void Load(std::string rsc_k, std::string mapName, std::map<std::string, Tileset>& tilesets);
    void Unload(std::map<std::string, Tileset>& tilesets);
    void Write(string rsc_k, string mapName);
    void Clear();
    void DrawLayer(u8 layerNum, std::map<string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool showAttr);
    void DrawEntities(std::map<u8, EntityDisplay>& entities, std::map<std::string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool debug = false);
};