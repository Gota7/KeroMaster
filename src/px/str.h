#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include "gbin/gtypes.h"

struct Str : GReadable, GWriteable {
    std::string dat;
    void Read(GFile* f);
    void Write(GFile* f);
};