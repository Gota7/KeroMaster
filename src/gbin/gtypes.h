#pragma once

#include "../types.h"
#include "gfile.h"
#include <stdio.h>

typedef void* (*readFunc)(GFile* f);

struct GReadable
{
    virtual void Read(GFile* f) = 0;
};

struct GWriteable
{
    virtual void Write(GFile* f) = 0;
    void* ReturnStuff();
};

template <typename T>
struct GTable
{
    u32 len;
    T* values;
    void Read(GFile* f, readFunc func);
    void Write(GFile* f, readFunc func);
    ~GTable();
};