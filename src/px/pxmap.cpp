#include "pxmap.h"
#include "../conv/conv.h"
#include "../ed/entityDisplay.h"
#include "tileset.h"

const char* PxMap::layerNames[] =
{
    "Foreground",
    "Middleground",
    "Background"
};

void PxMap::Read(GFile* f)
{

    // Unload map if it has been loaded.
    if (tiles != nullptr)
    {
        Unload();
    }

    // Check for valid data, then read.
    if (f->ReadStr() != "pxMAP01")
    {
        throw std::string("Invalid map attribute magic.");
    }
    else
    {
        width = newWidth = f->ReadU16();
        height = newHeight = f->ReadU16();

        if (width * height > 0) flags = f->ReadU8();

        tiles = new u8[width * height];
        for (int i = 0; i < width * height; i++)
        {
            tiles[i] = f->ReadU8();
        }
    }

}

void PxMap::Write(GFile* f)
{
    f->WriteNullTerminated("pxMAP01");
    f->Write(width);
    f->Write(height);

    if (width * height > 0) f->Write(flags);

    for (int i = 0; i < width * height; i++)
    {
        f->Write(tiles[i]);
    }
}

u8 PxMap::SetTile(u32 index, u8 newTile) 
{
    if (index > width * height) return 0;

    u8 oldTile = tiles[index];
    tiles[index] = newTile;
    return oldTile;
}

u8 PxMap::SetTile(u16 x, u16 y, u8 newTile)
{
    return SetTile(x + y * width, newTile);
}


u8 PxMap::GetTile(u32 index)
{
    if (index > width * height) return 0;

    return tiles[index];
}

u8 PxMap::GetTile(u16 x, u16 y)
{
    return GetTile(x + y * width);
}

void PxMap::Resize(u16 newWidth, u16 newHeight)
{
    u8* bak = tiles;
    tiles = new u8[newWidth * newHeight];
    memset(tiles, 0, newWidth * newHeight);
    for (int x = 0; x < std::min(newWidth, width); x++)
    {
        for (int y = 0; y < std::min(newHeight, height); y++)
        {
            tiles[x + y * newWidth] = bak[x + y * width];
        }
    }
    width = this->newWidth = newWidth;
    height = this->newHeight = newHeight;
    delete[] bak;
}

bool PxMap::CanShift(ShiftDirection dir)
{
    if (width <= 1 && (dir == ShiftDirection::Right || dir == ShiftDirection::Left))
    {
        return false;
    }
    if (height <= 1 && (dir == ShiftDirection::Up || dir == ShiftDirection::Down))
    {
        return false;
    }
    if (dir == ShiftDirection::Up)
    {
        for (int x = 0; x < width; x++)
        {
            if (GetTile(x, 0) != 0)
            {
                return false;
            }
        }
        return true;
    }
    else if (dir == ShiftDirection::Down)
    {
        for (int x = 0; x < width; x++)
        {
            if (GetTile(x, height - 1) != 0)
            {
                return false;
            }
        }
        return true;
    }
    else if (dir == ShiftDirection::Left)
    {
        for (int y = 0; y < height; y++)
        {
            if (GetTile(0, y) != 0)
            {
                return false;
            }
        }
        return true;
    }
    else if (dir == ShiftDirection::Right)
    {
        for (int y = 0; y < height; y++)
        {
            if (GetTile(width - 1, y) != 0)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void PxMap::Shift(ShiftDirection dir)
{
    if (dir == ShiftDirection::Up)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 1; y < height; y++)
            {
                tiles[x + (y - 1) * width] = tiles[x + y * width];
            }
        }
        for (int x = 0; x < width; x++)
        {
            tiles[x + (height - 1) * width] = 0;
        }
    }
    else if (dir == ShiftDirection::Down)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = height - 2; y >= 0; y--)
            {
                tiles[x + (y + 1) * width] = tiles[x + y * width];
            }
        }
        for (int x = 0; x < width; x++)
        {
            tiles[x] = 0;
        }
    }
    else if (dir == ShiftDirection::Left)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 1; x < width; x++)
            {
                tiles[(x - 1) + y * width] = tiles[x + y * width];
            }
        }
        for (int y = 0; y < height; y++)
        {
            tiles[width - 1 + y * width] = 0;
        }
    }
    else if (dir == ShiftDirection::Right)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = width - 2; x >= 0; x--)
            {
                tiles[(x + 1) + y * width] = tiles[x + y * width];
            }
        }
        for (int y = 0; y < height; y++)
        {
            tiles[y * width] = 0;
        }
    }
}

void PxMap::Unload()
{
    delete[] tiles;
    width = 0;
    height = 0;
    flags = 0;
}

void Map::Load(std::string rsc_k, std::string mapName, std::map<std::string, Tileset>& tilesets)
{
    std::string mapPath = rsc_k + "/field/" + mapName + ".pxpack";

    if (!GFile::FileExists(mapPath.c_str())) {
        throw std::string("File not found: ") + mapPath;
    }

    GFile f = GFile(mapPath.c_str());
    if (f.ReadStr() != "PXPACK121127a**")
    {
        throw std::string("Invalid pack attribute magic.");
    }
    else
    {
        levelTitle.Read(&f);
        levelTitle.dat = fromShiftJIS(levelTitle.dat);

        for (int i = 0; i < NUM_REFERENCES; i++)
        {
            references[i].Read(&f);
        }

        if (this->references[RT_NPC_PALETTE].dat != "")
        {
            Tileset t;
            t.Load(rsc_k, this->references[RT_NPC_PALETTE].dat);
            tilesets[this->references[RT_NPC_PALETTE].dat] = t;
        }

        for (int i = 0; i < NUM_SETTINGS; i++)
        {
            levelSettings[i] = f.ReadU8();
        }

        for (int i = 0; i < NUM_TILESETS; i++)
        {
            this->tilesets[i].Read(&f);
            if (this->tilesets[i].dat != "")
            {
                tilesets[this->tilesets[i].dat].Unload();
                Tileset t;
                t.Load(rsc_k, this->tilesets[i].dat);
                tilesets[this->tilesets[i].dat] = t;
            }
            tilesetSettings1[i] = f.ReadU8();
            tilesetSettings2[i] = f.ReadU8();
        }

        for (int i = 0; i < NUM_TILESETS; i++)
        {
            maps[i].Read(&f);
        }

        u16 numEntities = f.ReadU16();
        for (u16 i = 0; i < numEntities; i++)
        {
            Entity e;
            e.flags = f.ReadU8();
            e.id = f.ReadU8();
            e.variant = f.ReadU8();
            e.xPos = f.ReadU16();
            e.yPos = f.ReadU16();
            e.flag = f.ReadU16();
            e.data.Read(&f);
            entities.push_back(e);
        }
    }

    f.Close();
}

void Map::Unload(std::map<std::string, Tileset>& tilesets)
{
    if (this->references[RT_NPC_PALETTE].dat != "")
    {
        tilesets[this->references[RT_NPC_PALETTE].dat].Unload();
    }

    for (int i = 0; i < NUM_TILESETS; i++)
    {
        if (this->tilesets[i].dat != "")
        {
            tilesets[this->tilesets[i].dat].Unload();
        }
    }

    entities.clear();
}

void Map::Write(std::string rsc_k, std::string mapName)
{
    std::string mapPath = rsc_k + "/field/" + mapName + ".pxpack";
    GFile f = GFile(mapPath.c_str());
    f.Clear();
    f.WriteNullTerminated("PXPACK121127a**");

    Str sjisComment;
    sjisComment.dat = toShiftJIS(levelTitle.dat);
    sjisComment.Write(&f);

    for (int i = 0; i < NUM_REFERENCES; i++)
    {
        references[i].Write(&f);
    }
    for (int i = 0; i < NUM_SETTINGS; i++)
    {
        f.Write(levelSettings[i]);
    }
    for (int i = 0; i < NUM_TILESETS; i++)
    {
        tilesets[i].Write(&f);
        f.Write(tilesetSettings1[i]);
        f.Write(tilesetSettings2[i]);
    }

    for (int i = 0; i < NUM_TILESETS; i++)
    {
        maps[i].Write(&f);
    }

    f.Write((u16)entities.size());
    for (u16 i = 0; i < entities.size(); i++)
    {
        Entity* e = &entities[i];
        f.Write(e->flags);
        f.Write(e->id);
        f.Write(e->variant);
        f.Write(e->xPos);
        f.Write(e->yPos);
        f.Write(e->flag);
        e->data.Write(&f);
    }
    f.Close();
}

void Map::Clear()
{
    ClearBackground(GetPixelColor(&levelSettings[5], PIXELFORMAT_UNCOMPRESSED_R8G8B8));
}

float Map::TileSize(int layerNum)
{
    return tilesetSettings1[layerNum] == 0 ? Tileset::MAP_TILE_SIZE : ((float)16 / tilesetSettings1[layerNum]);
}

void Map::DrawLayer(u8 layerNum, std::map<std::string, Tileset>& tilesets, Vector2 origin, bool showAttr)
{
    if (layerNum >= NUM_TILESETS)
    {
        return;
    }

    PxMap& m = maps[layerNum];
    Tileset& t = tilesets[this->tilesets[layerNum].dat];

    for (u16 y = 0; y < m.height; y++)
    {
        for (u16 x = 0; x < m.width; x++)
        {
            t.Draw(m.GetTile(x, y), x, y, TileSize(layerNum), Tileset::EDITOR_TILE_SIZE, showAttr, false, false, origin);
        }
    }
}

void Map::DrawEntities(std::map<u8, EntityDisplay>& entities, std::map<std::string, Tileset>& tilesets, Vector2 origin, bool debug)
{
    std::string tilesetNames[3];
    tilesetNames[0] = this->tilesets[0].dat;
    tilesetNames[1] = this->tilesets[1].dat;
    tilesetNames[2] = this->tilesets[2].dat;
    for (u16 i = 0; i < this->entities.size(); i++)
    {

        // Draw ID if found, else draw nothing which will show the box.
        if (entities.find(this->entities[i].id) != entities.end())
        {
            entities[this->entities[i].id].Draw(&this->entities[i], tilesets, this->references[RT_NPC_PALETTE].dat, tilesetNames, debug, origin);
        }
        else
        {
            entities[0].Draw(&this->entities[i], tilesets, this->references[RT_NPC_PALETTE].dat, tilesetNames, debug, origin);
        }

    }
}

void Map::ImportLayer(std::string path, int layerNum)
{
    if (layerNum >= NUM_TILESETS)
    {
        return;
    }
    GFile f(path.c_str());
    maps[layerNum].Read(&f);
    f.Close();
}

void Map::ExportLayer(std::string path, int layerNum)
{
    if (layerNum >= NUM_TILESETS)
    {
        return;
    }
    GFile f(path.c_str());
    f.Clear();
    maps[layerNum].Write(&f);
    f.Close();
}

void Map::ImportEntities(std::string path)
{
    GFile f(path.c_str());
    if (f.ReadStr() != "kmENT01")
    {
        throw std::string("Invalid entity attribute magic.");
    }
    else
    {
        u16 numEntities = f.ReadU16();
        entities.clear();
        for (u16 i = 0; i < numEntities; i++)
        {
            Entity e;
            e.flags = f.ReadU8();
            e.id = f.ReadU8();
            e.variant = f.ReadU8();
            e.xPos = f.ReadU16();
            e.yPos = f.ReadU16();
            e.flag = f.ReadU16();
            e.data.Read(&f);
            entities.push_back(e);
        }
    }
    f.Close();
}

void Map::ExportEntities(std::string path)
{
    GFile f(path.c_str());
    f.Clear();
    f.WriteNullTerminated("kmENT01");
    f.Write((u16)entities.size());
    for (u16 i = 0; i < entities.size(); i++)
    {
        Entity* e = &entities[i];
        f.Write(e->flags);
        f.Write(e->id);
        f.Write(e->variant);
        f.Write(e->xPos);
        f.Write(e->yPos);
        f.Write(e->flag);
        e->data.Write(&f);
    }
    f.Close();
}