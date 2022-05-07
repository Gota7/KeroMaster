#include "scriptEditor.h"
#include "editor.h"
#include "../gbin/gfile.h"
#include "../rlImGui/utils.h"
#include "../tinyxml2/tinyxml2.h"

bool ScriptEditor::loadedGuide = false;
ScriptHelpData* ScriptEditor::commands;
int ScriptEditor::numCommands = 0;

ScriptEditor::ScriptEditor(Editor* ed, std::string scriptName)
{
    this->ed = ed;
    name = scriptName;
    if (!loadedGuide)
    {
        LoadXML();
        loadedGuide = true;
    }
    LoadScript();
    open = true;
}

void ScriptEditor::LoadXML()
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result;

    if ((result = doc.LoadFile("object_data/scriptInfo.xml")) != tinyxml2::XML_SUCCESS)
    {
        throw std::string("Failed to load script info file file: ") + doc.ErrorIDToName(result);
    }

    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* c = root->FirstChildElement();
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
        tinyxml2::XMLElement* a = c->FirstChildElement();
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
    GFile f;
    if (GFile::FileExists((ed->rsc + "/text/" + name + ".pxeve").c_str()))
    {
        f = GFile((ed->rsc + "/text/" + name + ".pxeve").c_str());
    }
    else
    {
        f = GFile("object_data/default.pxeve");
    }
    buf.resize(f.Size() + 1);
    f.Read(buf.begin(), buf.size() - 1);
    buf[buf.size() - 1] = 0;
    f.Close();
}

void ScriptEditor::SaveScript()
{
    GFile f = GFile((ed->rsc + "/text/" + name + ".pxeve").c_str());
    f.Clear();
    f.Write(buf.begin(), strlen(buf.begin()));
    f.Close();
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
    ImGui::SetWindowSize(ImVec2(1000, 500), ImGuiCond_FirstUseEver);
    ed->focus.ObserveFocus();
    if (ImGui::Button("Save"))
    {
        SaveScript();
    }
    ImGui::Columns(2);
    ImGui::InputTextMultiline("##Script", buf.begin(), buf.size(), ImVec2(-FLT_MIN, ImGui::GetWindowSize().y - ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 4), ImGuiInputTextFlags_Multiline | ImGuiInputTextFlags_CallbackResize, ResizeCallback, &buf);
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
        std::string txt = c->name + " - " + c->description;
        if (c->numArgs == 0)
        {
            ImGui::Text("%s", (" * " + txt).c_str());
        }
        else 
        {
            if (ImGui::TreeNodeEx(txt.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (int j = 0; j < c->numArgs; j++)
                {
                    ScriptHelpArg* a = &c->args[j];
                    ImGui::Text("%s", (" " + std::to_string(j) + ": " + (a->type == ARG_TYPE_STRING ? "String: " : "Number: ") + a->description).c_str());
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();

}

void ScriptEditor::Close()
{
    buf.clear();
    open = false;
}

int ResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
        IM_ASSERT(my_str->begin() == data->Buf);
        my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
        data->Buf = my_str->begin();
    }
    return 0;
}