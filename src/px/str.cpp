#include "str.h"

void Str::Read(GFile* f)
{
    u8 len = f->ReadU8();
    dat = f->ReadStrFixed(len);
}

void Str::Write(GFile* f)
{
    if (dat.length() > 255)
    {
        printf("String length can't be more than 255!");
        throw new std::exception();
        return;
    }
    else
    {
        f->Write((u8)dat.length());
        for (int i = 0; i < dat.length(); i++)
        {
            f->Write((u8)dat[i]);
        }
    }
}