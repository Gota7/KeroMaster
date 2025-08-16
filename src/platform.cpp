#include "platform.h"
#include <filesystem>
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#else
#include <cstdlib>
#include <cstring>
#endif

namespace fs = std::filesystem;

std::filesystem::path GetKMUserPath()
{
#if defined(_WIN32)
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path)) && path)
    {
        std::wstring wpath(path);
        CoTaskMemFree(path);
        auto result = fs::path(wpath) / "KeroMaster";
        return result;
    }

    throw std::runtime_error("Failed to get user path");
#elif defined(__APPLE__)
#error This unit shouldn't be built on Apple platforms.
#else
    auto getenv_str = [](const char *var)
    {
        const char *value = std::getenv(var);
        return value ? std::string(value) : std::string();
    };

    // XDG Base Directory spec (Linux, FreeBSD, etc)
    auto xdg = getenv_str("XDG_CONFIG_HOME");
    if (!xdg.empty())
    {
        return fs::path(xdg) / "KeroMaster";
    }
    auto home = getenv_str("HOME");
    if (!home.empty())
    {
        return fs::path(home) / ".config" / "KeroMaster";
    }

    throw std::runtime_error("Failed to get user path");
#endif
}