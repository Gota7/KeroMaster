#include "utils.h"
#include "raylib.h"

std::vector<std::string> ReadFilesFromDir(std::string path, bool includeExt)
{
    int numFiles;
    char** files = GetDirectoryFiles(path.c_str(), &numFiles);
    std::vector<std::string> ret;
    for (int i = 0; i < numFiles; i++)
    {
        if (strcmp(files[i], ".") != 0 && strcmp(files[i], "..") != 0)
        {
            if (includeExt)
            {
                ret.push_back(files[i]);
            }
            else
            {
                ret.push_back(GetFileNameWithoutExt(files[i]));
            }
        }
    }
    return ret;
}

char* ConvertStrRef(const std::string& s)
{
   char* pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   return pc; 
}

char** GenImGuiStringList(std::vector<std::string>& strings, int* outStringCount)
{
    char** ret;
    *outStringCount = strings.size();
    ret = new char*[*outStringCount];
    for (int i = 0; i < *outStringCount; i++)
    {
        ret[i] = ConvertStrRef(strings[i]);
    }
    return ret;
}

void DelImGuiStringList(char** ptr, int stringCount)
{
    for (int i = 0; i < stringCount; i++)
    {
        delete[] ptr[i];
    }
    delete[] ptr;
}

void ImGuiNumEdit(std::string name, s64& data, size_t min, size_t max, std::string format)
{
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton(("##" + name + "left").c_str(), ImGuiDir_Left) && data > min) { data--; }
    ImGui::SameLine(0.0f, spacing);
    ImGui::Text(format.c_str(), data);
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton(("##right" + name + "left").c_str(), ImGuiDir_Right) && data < max) { data++; }
    ImGui::PopButtonRepeat();
}

void ImGuiTooltip(std::string tip)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::SetTooltip("%s", tip.c_str());
        ImGui::EndTooltip();
    }
}

bool ImGuiStringEdit(const char* label, std::string* toEdit)
{
    char buf[128];
    strcpy(buf, &(*toEdit)[0]);
    bool ret = ImGui::InputText(label, buf, 128);
    *toEdit = std::string(buf);
    return ret;
}

void ImGuiColorEdit(const char* label, u8* toEdit)
{
    float tmp[3];
    tmp[0] = toEdit[0] / (float)255;
    tmp[1] = toEdit[1] / (float)255;
    tmp[2] = toEdit[2] / (float)255;
    if (ImGui::ColorEdit3(label, tmp))
    {
        toEdit[0] = (u8)(tmp[0] * 255);
        toEdit[1] = (u8)(tmp[1] * 255);
        toEdit[2] = (u8)(tmp[2] * 255);
    }
}