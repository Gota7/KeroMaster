#include "entityDisplay.h"

map<string, EntityTile> EntityDisplay::rollYourOwnSprite;
float EntityDisplay::transparency = .3f;

void EntityDisplay::Draw(u8 id, Str strParam, u8 flags, bool beingEdited, string spritesheetName, string tilesetNames[3], map<string, Tileset>& tilesets, Vector2 origin, s32 xOff, s32 yOff, f32 mapScale, bool debug)
{
    Vector2 off = { origin.x + xOff * 8 * mapScale, origin.y + yOff * 8 * mapScale };
    Vector2 size = { 8 * mapScale, 8 * mapScale };
    bool noTiles = numTiles == 0;
    bool blankSpriteParam = strcmp(strParam.dat.c_str(), "") == 0;
    bool spriteExistsInRollList = rollYourOwnSprite.find(strParam.dat) != rollYourOwnSprite.end();
    bool doneSomething = false;

    if(!noTiles)
    {
        for (int i = 0; i < numTiles; i++)
        {
            EntityTile* t = &tiles[i];
            if (t->flagBit != 0)
            {
                u8 val = flags & (1 << (t->flagBit - 1));
                if ((val > 0) == t->flagMode)
                {
                    continue;
                }
            }

            string tsName = strcmp(t->tileset.c_str(), "") == 0 ? spritesheetName : t->tileset;
            if (strcmp(t->tileset.c_str(), "/0") == 0)
            {
                tsName = tilesetNames[0];
            }
            if (strcmp(t->tileset.c_str(), "/1") == 0)
            {
                tsName = tilesetNames[1];
            }
            if (strcmp(t->tileset.c_str(), "/2") == 0)
            {
                tsName = tilesetNames[2];
            }
            Tileset* ts = &tilesets[tsName];
            for (s16 y = 0; y < t->numTilesY; y++)
            {
                for (s16 x = 0; x < t->numTilesX; x++)
                {
                    ts->Draw(t->x + x, t->y + y, origin, xOff + x + t->xOff, yOff + y + t->yOff, mapScale, false, true, t->xOffPixels, t->yOffPixels);
                }
            }
        }
        doneSomething = true;
    }
    if (allowRollYourOwnSprite && !blankSpriteParam)
    {
        if (spriteExistsInRollList)
        {
            EntityTile* t = &rollYourOwnSprite[strParam.dat];
            if (t->flagBit != 0)
            {
                u8 val = flags & (1 << (t->flagBit - 1));
                if ((val > 0) == t->flagMode)
                {
                    goto skip;
                }
            }
            string tsName = strcmp(t->tileset.c_str(), "") == 0 ? spritesheetName : t->tileset;
            if (strcmp(t->tileset.c_str(), "/0") == 0)
            {
                tsName = tilesetNames[0];
            }
            if (strcmp(t->tileset.c_str(), "/1") == 0)
            {
                tsName = tilesetNames[1];
            }
            if (strcmp(t->tileset.c_str(), "/2") == 0)
            {
                tsName = tilesetNames[2];
            }
            Tileset* ts = &tilesets[tsName];
            for (s16 y = 0; y < t->numTilesY; y++)
            {
                for (s16 x = 0; x < t->numTilesX; x++)
                {
                    ts->Draw(t->x + x, t->y + y, origin, xOff + x + t->xOff, yOff + y + t->yOff, mapScale, false, true, t->xOffPixels, t->yOffPixels);
                }
            }
            doneSomething = true;
        }
    }
    skip:
    if (!doneSomething)
    {
        DrawTextureTiled(Tileset::unitType, { (float)(id % 16 * 16), (float)(id / 16 * 16), 16, 16 }, { off.x, off.y, 8 * mapScale, 8 * mapScale }, { 0, 0 }, 0, mapScale / 2, WHITE);
        doneSomething = true;
    }
    if (debug || !doneSomething)
    {
        if (!beingEdited)
        {
            DrawRectangleV(off, size, ColorAlpha(BLUE, transparency));
        }
        else
        {
            DrawRectangleV(off, size, ColorAlpha(RED, transparency + .2f));
        }
        DrawText(to_string(id).c_str(), (int)(off.x + 1 * mapScale), (int)(off.y + 1 * mapScale), (int)(5 * mapScale), ColorAlpha(YELLOW, .8f));
    }
}

map<u8, EntityDisplay> LoadXML(string game)
{
    map<u8, EntityDisplay> ret;
    XMLDocument doc;
    XMLError result;

    if ((result = doc.LoadFile(("object_data/" + game + ".xml").c_str())) != XML_SUCCESS) {
        throw string("Failed to load object data definition file: ") + doc.ErrorIDToName(result);
    }

    XMLElement* root = doc.RootElement();
    XMLElement* entities = root->FirstChildElement("entities");
    XMLElement* e = entities->FirstChildElement();

    while (e != nullptr)
    {
        EntityDisplay d;
        d.name = e->Attribute("name");
        d.description = e->Attribute("description");
        XMLElement* t = e->FirstChildElement();

        while (t != nullptr)
        {
            d.numTiles++;
            t = t->NextSiblingElement();
        }
        
        d.tiles = new EntityTile[d.numTiles];
        t = e->FirstChildElement();
        int num = 0;
        while (t != nullptr)
        {
            EntityTile dt;

            dt.tileset = t->Attribute("tileset");
            dt.x = (u16)t->IntAttribute("x");
            dt.y = (u16)t->IntAttribute("y");
            dt.xOff = t->IntAttribute("xOff");
            dt.yOff = t->IntAttribute("yOff");
            dt.xOffPixels = (s8)t->IntAttribute("xOffPixels");
            dt.yOffPixels = (s8)t->IntAttribute("yOffPixels");
            dt.numTilesX = (u16)t->IntAttribute("numXTiles");
            dt.numTilesY = (u16)t->IntAttribute("numYTiles");
            dt.flagMode = t->BoolAttribute("flagMode");
            dt.flagBit = (u8)t->IntAttribute("flagBit");
            d.tiles[num] = dt;
            num++;
            t = t->NextSiblingElement();
        }

        ret[e->IntAttribute("id")] = d;
        e = e->NextSiblingElement();
    }

    XMLElement* rollSprites = root->FirstChildElement("rollYourOwnSprites");
    XMLElement* r = rollSprites->FirstChildElement();
    while (r != NULL)
    {
        EntityTile d;
        d.tileset = r->Attribute("tileset");
        d.x = (u16)r->IntAttribute("x");
        d.y = (u16)r->IntAttribute("y");
        d.xOff = r->IntAttribute("xOff");
        d.yOff = r->IntAttribute("yOff");
        d.xOffPixels = (s8)r->IntAttribute("xOffPixels");
        d.yOffPixels = (s8)r->IntAttribute("yOffPixels");
        d.numTilesX = (u16)r->IntAttribute("numXTiles");
        d.numTilesY = (u16)r->IntAttribute("numYTiles");
        EntityDisplay::rollYourOwnSprite[r->Attribute("name")] = d;
        r = r->NextSiblingElement();
    }
    
    return ret;
}