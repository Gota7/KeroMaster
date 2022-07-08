#include "entityDisplay.h"
#include "settings.h"
#include "../px/tileset.h"
#include "../tinyxml2/tinyxml2.h"

std::map<std::string, std::vector<EntityTile>> EntityDisplay::rollYourOwnSprite;
float EntityDisplay::transparency = .3f;

void EntityDisplay::DrawEntityIdBox(u8 id, s32 tileX, s32 tileY, bool beingEdited, Vector2 offset)
{
    Rectangle destRec = Tileset::GetDestRect(tileX, tileY, Tileset::EDITOR_TILE_SIZE, offset);
    if (!beingEdited)
    {
        DrawRectangleRec(destRec, ColorAlpha(BLUE, transparency));
    }
    else
    {
        DrawRectangleRec(destRec, ColorAlpha(RED, transparency + .2f));
    }
    DrawText(std::to_string(id).c_str(), (int)(destRec.x + 1), (int)(destRec.y + 1), 1, ColorAlpha(YELLOW, .8f));
}

void EntityDisplay::DrawEntityUnitType(u8 id, s32 tileX, s32 tileY, Vector2 offset)
{
    DrawTexturePro(
        Tileset::unitType,
        Tileset::GetSrcRect(id, (f32)Tileset::UNIT_TYPE_TILE_SIZE, Tileset::unitType.width / Tileset::UNIT_TYPE_TILE_SIZE),
        Tileset::GetDestRect(tileX, tileY, Tileset::EDITOR_TILE_SIZE, offset),
        { 0, 0 },
        0,
        WHITE
    );
}

void EntityDisplay::DrawEntityTiles(Tileset& ts, u16 x, u16 y, u16 width, u16 height, s32 tileX, s32 tileY, Vector2 offset)
{

    // Entity tiles may be smaller than map tiles.
    tileX *= Tileset::MAP_TILE_SIZE / Tileset::ENTITY_TILE_SIZE;
    tileY *= Tileset::MAP_TILE_SIZE / Tileset::ENTITY_TILE_SIZE;

    // Go ahead and draw all the tiles.
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            ts.Draw(
                x + i,
                y + j,
                tileX + i,
                tileY + j,
                Tileset::ENTITY_TILE_SIZE,
                Tileset::EDITOR_TILE_SIZE / (Tileset::MAP_TILE_SIZE / Tileset::ENTITY_TILE_SIZE),
                false,
                true,
                true,
                offset
            );
        }
    }

}

void EntityDisplay::DrawEntityTile(EntityTile* tile, std::map<std::string, Tileset>& loadedTilesets, std::string spriteSheet, std::string tilesetNames[3], s32 tileX, s32 tileY, Vector2 offset)
{

    // Get the tileset name.
    std::string tsName = tile->tileset == "" ? spriteSheet : tile->tileset;
    if (tile->tileset == "")
    {
        tsName = spriteSheet;
    }
    else if (tile->tileset == "/0")
    {
        tsName = tilesetNames[0];
    }
    else if (tile->tileset == "/1")
    {
        tsName = tilesetNames[1];
    }
    else if (tile->tileset == "/2")
    {
        tsName = tilesetNames[2];
    }
    else
    {
        tsName = tile->tileset;
    }

    // Then draw the correct entity tiles.
    float entityToEditorRatio = Tileset::EDITOR_TILE_SIZE / (float)Tileset::ENTITY_TILE_SIZE;
    DrawEntityTiles(
        loadedTilesets[tsName],
        tile->x,
        tile->y,
        tile->numTilesX,
        tile->numTilesY,
        tileX,
        tileY,
        {
            tile->xOff * Tileset::ENTITY_TILE_SIZE * entityToEditorRatio + tile->xOffPixels * (Tileset::EDITOR_TILE_SIZE / Tileset::MAP_TILE_SIZE) + offset.x,
            tile->yOff * Tileset::ENTITY_TILE_SIZE * entityToEditorRatio + tile->yOffPixels * (Tileset::EDITOR_TILE_SIZE / Tileset::MAP_TILE_SIZE) + offset.y
        }
    );

}

void EntityDisplay::Draw(Entity* entity, std::map<std::string, Tileset>& loadedTilesets, std::string spriteSheet, std::string tilesetNames[3], bool debug, Vector2 offset)
{

    // Get entity info.
    std::string& strParam = entity->data.dat;
    bool noTiles = numTiles == 0;
    bool blankSpriteParam = strParam == "";
    bool spriteExistsInRollList = rollYourOwnSprite.find(strParam) != rollYourOwnSprite.end();
    bool drewSomething = false;

    // Draw tiles if they exist.
    if (!noTiles)
    {
        for (int i = 0; i < numTiles; i++)
        {

            // Get tile, and check to see if we don't draw on this flag.
            EntityTile* t = &tiles[i];
            if (t->flagBit)
            {
                u8 val = entity->flags & (1 << (t->flagBit - 1));
                if ((val > 0) != t->flagMode)
                {
                    continue;
                }
            }

            // Check to see if we don't draw on this variant parameter.
            if (t->variantDraw && t->variantDraw - 1 != entity->variant)
            {
                continue;
            }

            // Draw the tile.
            DrawEntityTile(
                t,
                loadedTilesets,
                spriteSheet,
                tilesetNames,
                entity->xPos,
                entity->yPos,
                offset
            );
            drewSomething = true;

        }
    }

    // Roll your own sprite.
    if (allowRollYourOwnSprite && !blankSpriteParam && spriteExistsInRollList)
    {
        for (int i = 0; i < rollYourOwnSprite[strParam].size(); i++)
        {

            // Get tile to draw, and exit if it isn't supposed to be drawn for this flag.
            EntityTile* t = &rollYourOwnSprite[strParam][i];
            if (t->flagBit != 0)
            {
                u8 val = entity->flags & (1 << (t->flagBit - 1));
                if ((val > 0) != t->flagMode)
                {
                    continue;
                }
            }

            // Check to see if we don't draw on this variant parameter.
            if (t->variantDraw && t->variantDraw - 1 != entity->variant)
            {
                continue;
            }

            // Draw the tile.
            DrawEntityTile(
                t,
                loadedTilesets,
                spriteSheet,
                tilesetNames,
                entity->xPos,
                entity->yPos,
                offset
            );
            drewSomething = true;

        }
    }

    // Draw unit type and/or boxes if appropriate:
    if (!drewSomething)
    {
        DrawEntityUnitType(entity->id, entity->xPos, entity->yPos, offset);
        drewSomething = true;
    }
    if (debug || !drewSomething)
    {
        DrawEntityIdBox(entity->id, entity->xPos, entity->yPos, entity->beingEdited, offset);
    }

}

std::map<u8, EntityDisplay> LoadXML(Settings* settings)
{
    std::map<u8, EntityDisplay> ret;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result;

    if ((result = doc.LoadFile(settings->entityDataPath.c_str())) != tinyxml2::XML_SUCCESS) {
        throw std::string("Failed to load object data definition file: ") + doc.ErrorIDToName(result);
    }

    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* entities = root->FirstChildElement("entities");
    tinyxml2::XMLElement* e = entities->FirstChildElement();

    while (e != nullptr)
    {
        EntityDisplay d;
        d.name = e->Attribute("name");
        d.description = e->Attribute("description");
        tinyxml2::XMLElement* t = e->FirstChildElement();

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
            dt.variantDraw = (u16)t->IntAttribute("variantDraw");
            d.tiles[num] = dt;
            num++;
            t = t->NextSiblingElement();
        }

        ret[e->IntAttribute("id")] = d;
        e = e->NextSiblingElement();
    }

    tinyxml2::XMLElement* rollSprites = root->FirstChildElement("rollYourOwnSprites");
    tinyxml2::XMLElement* r = rollSprites->FirstChildElement();
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
        d.flagMode = r->BoolAttribute("flagMode");
        d.flagBit = (u8)r->IntAttribute("flagBit");
        d.variantDraw = (u16)r->IntAttribute("variantDraw");
        EntityDisplay::rollYourOwnSprite[r->Attribute("name")].push_back(d);
        r = r->NextSiblingElement();
    }

    return ret;
}