#pragma once

#include <string>
#include <string.h>
#include <raylib.h>
#include "gbin/gtypes.h"

using namespace std;

struct Str : GReadable, GWriteable {
    string dat;
    void Read(GFile* f);
    void Write(GFile* f);
};