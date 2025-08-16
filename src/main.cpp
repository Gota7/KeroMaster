#include "ed/editor.h"
#include "platform.h"
#include <filesystem>

int main(int argc, char *argv[])
{
    auto userDir = GetKMUserPath();

    std::filesystem::create_directories(userDir);
    std::filesystem::current_path(userDir);
    std::filesystem::create_directories(userDir / "object_data");
    std::filesystem::create_directories(userDir / "themes");

    Editor editor;
    editor.Init();
    return editor.EditorLoop();
}