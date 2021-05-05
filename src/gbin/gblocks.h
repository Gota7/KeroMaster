#pragma once

#include "types.h"
#include "gtypes.h"

#include <stdio.h>

// Reference structure.
struct Reference : public GReadable, public GWriteable
{
    u16 type;
    s64 off;
    u64 size;
};