#pragma once

#include <string>

std::string fromShiftJIS(std::string& in_str);
std::string fromShiftJIS(const char* data, unsigned int length);

std::string toShiftJIS(std::string& in_str);
std::string toShiftJIS(const char* data, unsigned int length);