#include "pxmap.h"

void PxMap::Read(GFile* f)
{
    if (tiles != nullptr) {
        Unload();
    }

    if (strcmp(f->ReadStr().c_str(), "pxMAP01"))
    {
        throw string("Invalid map attribute magic.");
    }
    else
    {
        width = f->ReadU16();
        height = f->ReadU16();

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

u8 PxMap::GetTile(u8 index)
{
    return tiles[index];
}

u8 PxMap::GetTile(u8 x, u8 y)
{
    return tiles[x + y * width];
}

void PxMap::Unload()
{
    delete[] tiles;
    width = 0;
    height = 0;
    flags = 0;
}

void Map::Load(string rsc_k, string mapName, map<string, Tileset>& tilesets)
{
    string mapPath = rsc_k + "/field/" + mapName + ".pxpack";

    if (!GFile::FileExists(mapPath.c_str())) {
        throw string("File not found: ") + mapPath;
    }

    GFile f = GFile(mapPath.c_str());
    if (strcmp(f.ReadStr().c_str(), "PXPACK121127a**"))
    {
        throw string("Invalid pack attribute magic.");
    }
    else
    {
        comment.Read(&f);
        comment.dat = fromShiftJIS(comment.dat);

        for (int i = 0; i < NUM_REFERENCES; i++)
        {
            references[i].Read(&f);
        }
        
        if (strcmp(this->references[RT_NPC_PALETTE].dat.c_str(), "") != 0)
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
            if (strcmp(this->tilesets[i].dat.c_str(), "") != 0)
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
            e.unk = f.ReadU8();
            e.xPos = f.ReadU16();
            e.yPos = f.ReadU16();
            for (int i = 0; i < NUM_BYTE_PARAMETERS; i++)
            {
                e.parametersByte[i] = f.ReadU8();
            }
            for (int i = 0; i < NUM_PARAMETERS - NUM_BYTE_PARAMETERS; i++)
            {
                e.parametersStr[i].Read(&f);
            }
            entities.push_back(e);
        }
    }

    f.Close();
}

void Map::Unload(map<string, Tileset>& tilesets)
{
    if (strcmp(this->references[RT_NPC_PALETTE].dat.c_str(), "") != 0)
    {
        tilesets[this->references[RT_NPC_PALETTE].dat].Unload();
    }

    for (int i = 0; i < NUM_TILESETS; i++)
    {
        if (strcmp(this->tilesets[i].dat.c_str(), "") != 0)
        {
            tilesets[this->tilesets[i].dat].Unload();
        }
    }
    
    entities.clear();
}

void Map::Write(string rsc_k, string mapName)
{
    string mapPath = rsc_k + "/field/" + mapName + ".pxpack";
    GFile f = GFile(mapPath.c_str());
    f.Clear();
    f.WriteNullTerminated("PXPACK121127a**");
    comment.Write(&f);
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
        f.Write(e->unk);
        f.Write(e->xPos);
        f.Write(e->yPos);
        for (int i = 0; i < NUM_BYTE_PARAMETERS; i++)
        {
            f.Write(e->parametersByte[i]);
        }
        for (int i = 0; i < NUM_PARAMETERS - NUM_BYTE_PARAMETERS; i++)
        {
            e->parametersStr[i].Write(&f);
        }
    }
}

void Map::Clear()
{
    ClearBackground(GetPixelColor(&levelSettings[5], PIXELFORMAT_UNCOMPRESSED_R8G8B8));
}

void Map::DrawLayer(u8 layerNum, map<string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool showAttr)
{
    if (layerNum >= NUM_TILESETS)
    {
        return;
    }
    PxMap* m = &maps[layerNum];
    Tileset* t = &tilesets[this->tilesets[layerNum].dat];
    t->textureScale = tilesetSettings1[layerNum];
    if (t->textureScale == 0) {
        t->textureScale = 2;
    }

    for (u16 y = 0; y < m->height; y++)
    {
        for (u16 x = 0; x < m->width; x++)
        {
            t->Draw(m->GetTile(x, y), origin, x, y, mapScale, showAttr);
        }
    }
}

void Map::DrawEntities(map<u8, EntityDisplay>& entities, map<string, Tileset>& tilesets, Vector2 origin, f32 mapScale, bool debug)
{
    string tilesetNames[3];
    tilesetNames[0] = this->tilesets[0].dat;
    tilesetNames[1] = this->tilesets[1].dat;
    tilesetNames[2] = this->tilesets[2].dat;
    for (u16 i = 0; i < this->entities.size(); i++)
    {
        if (entities.find(this->entities[i].id) != entities.end())
        {
            entities[this->entities[i].id].Draw(this->entities[i].id, this->entities[i].parametersStr[0], this->entities[i].flags, this->entities[i].beingEdited, this->references[RT_NPC_PALETTE].dat, tilesetNames, tilesets, origin, this->entities[i].xPos, this->entities[i].yPos, mapScale, debug);
        }
        else
        {
            entities[0].Draw(this->entities[i].id, this->entities[i].parametersStr[0], this->entities[i].flags, this->entities[i].beingEdited, this->references[RT_NPC_PALETTE].dat, tilesetNames, tilesets, origin, this->entities[i].xPos, this->entities[i].yPos, mapScale, debug);
        }
    }
}