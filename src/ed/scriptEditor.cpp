#include "scriptEditor.h"
#include "../rlImGui/utils.h"

using namespace std;

bool ScriptEditor::loadedGuide = false;

ScriptEditor::ScriptEditor(Editor* ed, string scriptName)
{
    this->ed = ed;
    name = scriptName;
    open = true;
    memset(buf, 0, bufSize);
    if (!loadedGuide)
    {
        LoadXML();
        loadedGuide = true;
    }
    LoadScript();
}

void ScriptEditor::LoadXML()
{
    XMLDocument doc;
    XMLError result;

    if ((result = doc.LoadFile("object_data/scriptInfo.xml")) != XML_SUCCESS)
    {
        throw string("Failed to load script info file file: ") + doc.ErrorIDToName(result);
    }

    XMLElement* root = doc.RootElement();
    XMLElement* c = root->FirstChildElement();
    numCommands = 0;
    while (c != nullptr)
    {
        numCommands++;
        c = c->NextSiblingElement();
    }
    c = root->FirstChildElement();
    commands = new ScriptHelpData[numCommands];
    int currCommand = 0;
    while (c != nullptr)
    {
        commands[currCommand].name = c->Attribute("name");
        commands[currCommand].description = c->Attribute("desc");
        commands[currCommand].numArgs = 0;
        XMLElement* a = c->FirstChildElement();
        while (a != nullptr)
        {
            commands[currCommand].numArgs++;
            a = a->NextSiblingElement();
        }
        commands[currCommand].args = new ScriptHelpArg[commands[currCommand].numArgs];
        int currArg = 0;
        a = c->FirstChildElement();
        while (a != nullptr)
        {
            commands[currCommand].args[currArg].type = strcmp(a->Attribute("type"), "str") == 0 ? ARG_TYPE_STRING : ARG_TYPE_NUMBER;
            commands[currCommand].args[currArg].description = a->Attribute("desc");
            currArg++;
            a = a->NextSiblingElement();
        }
        currCommand++;
        c = c->NextSiblingElement();
    }
}

void ScriptEditor::LoadScript()
{

}

void ScriptEditor::SaveScript()
{
    
}

void ScriptEditor::DrawUI()
{

    // Safety.
    if (!open)
    {
        return;
    }

    // Layout.
    ImGui::Begin(("Script - " + name).c_str(), &open);
    ed->focus.ObserveFocus();
    if (ImGui::Button("Save"))
    {
        SaveScript();
    }
    ImGui::Columns(2);
    // TODO: RESIZABLE INPUT!!!
    ImGui::InputTextMultiline("##Script", buf, bufSize, ImVec2(-FLT_MIN, ImGui::GetWindowSize().y - ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 4), ImGuiInputTextFlags_Multiline, NULL);
    ImGui::NextColumn();
    ImGui::InputText("Search", search, 5);
    for (int i = 0; i < numCommands; i++)
    {
        ScriptHelpData* c = &commands[i];
        bool skip = false;
        for (int j = 0; j < strlen(search); j++)
        {
            if (search[j] != c->name[j])
            {
                skip = true;
            }
        }
        if (skip)
        {
            continue;
        }
        string txt = c->name + " - " + c->description;
        if (c->numArgs == 0)
        {
            ImGui::Text((" * " + txt).c_str());
        }
        else 
        {
            if (ImGui::TreeNodeEx(txt.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (int j = 0; j < c->numArgs; j++)
                {
                    ScriptHelpArg* a = &c->args[j];
                    ImGui::Text((" " + to_string(j) + ": " + (a->type == ARG_TYPE_STRING ? "String: " : "Number: ") + a->description).c_str());
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();

}

void ScriptEditor::Close()
{
    open = false;
}