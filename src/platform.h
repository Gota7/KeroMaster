#pragma once

#include <filesystem>

/**
 * Returns a path to the app-specific directory for user configuration/data files.
 */
std::filesystem::path GetKMUserPath();