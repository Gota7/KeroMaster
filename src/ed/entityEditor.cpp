#include "entityEditor.h"
#include <vector>
#include "entityDisplay.h"
#include "editor.h"
#include "../rlImGui/utils.h"
#include "imgui.h"

EntityEditor::EntityEditor(Editor* ed)
{
    this->ed = ed;
}

void EntityEditor::LoadEntityListing(std::string xmlName)
{
    entities = LoadXML(xmlName);
    std::vector<std::string> defaultListing;
    std::fstream f;
    f.open("object_data/unittype.txt", std::ios::in);
    for (u16 i = 0; i <= 255; i++)
    {
        std::string defaultStr;
        std::getline(f, defaultStr);
        defaultListing.push_back(std::to_string(i) + " - " + (entities.find(i) != entities.end() ? entities[i].name : defaultStr));
    }
    f.close();
    if (entityListing != nullptr) DelImGuiStringList(entityListing, numEntities);
    entityListing = GenImGuiStringList(defaultListing, &numEntities);
}

void EntityEditor::DrawUI()
{

    // Nothing to edit.
    if (!editingEntity) return;

    // Init entity editor.
    ImGui::Begin("Entity Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(10.0f, 80.0f), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();

    // Entity ID.
    const int itemWidth = 150;
    int currId = editingEntity->id;
    EntityDisplay* d = &entities[currId];
    ImGui::PushItemWidth(itemWidth);
    if (ImGui::Combo("Entity Id", &currId, entityListing, numEntities))
    {
        editingEntity->id = currId;
    }
    if (d->description != "")
    {
        ImGuiTooltip(d->description);
    }

    // Entity flags.
    EditFlagBit(editingEntity->flags, 0, "Alive", "If the entity will spawn alive.", itemWidth);
    EditFlagBit(editingEntity->flags, 1, "Unk1", "Unknown flag.", itemWidth);
    EditFlagBit(editingEntity->flags, 2, "Hit Event", "Run an event when the entity is hit.", itemWidth);
    EditFlagBit(editingEntity->flags, 3, "Death Event", "Run an event when the entity dies.", itemWidth);
    EditFlagBit(editingEntity->flags, 4, "Alt Skin", "Use the alternate skin of the entity or direction.", itemWidth);
    EditFlagBit(editingEntity->flags, 5, "Unk2", "Unknown flag.", itemWidth);
    EditFlagBit(editingEntity->flags, 6, "Unset Flag Spawn", "If a flag is not set, spawn the entity.", itemWidth);
    EditFlagBit(editingEntity->flags, 7, "Set Flag Spawn", "If a flag is set, spawn the entity.", itemWidth);

    // Unknown parameter.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Variant", ImGuiDataType_U8, &editingEntity->variant);
    ImGuiTooltip("Used for different skins of entities.");

    // Byte parameters.
    ImGui::PushItemWidth(itemWidth);
    ImGui::InputScalar("Flag", ImGuiDataType_U16, &editingEntity->flag);
    ImGuiTooltip("Flag for NPC to spawn/not spawn on.\nIt can also just be a flag parameter in general.");

    //String parameter.
    ImGui::PushItemWidth(itemWidth);
    ImGuiStringEdit("Data", &editingEntity->data.dat);
    ImGuiTooltip("Additional entity data.\nCan be used for \"roll your own sprites\" or accessing through a script.");
    ImGui::End();

}

void EntityEditor::EditFlagBit(u8& dat, u8 bit, std::string name, std::string desc, int itemWidth)
{
    bool set = dat & (0b1 << bit);
    ImGui::PushItemWidth(itemWidth);
    if (ImGui::Checkbox(name.c_str(), &set))
    {
        dat &= ~(0b1 << bit);
        if (set) dat |= 0b1 << bit;
    }
    ImGuiTooltip(desc);
}