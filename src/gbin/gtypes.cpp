#include "gtypes.h"

template <typename T>
void GTable<T>::Read(GFile* f, readFunc func)
{
    len = f->ReadU32();
    values = new T[len];
    for (u32 i = 0; i < len; i++)
    {
        values[i] = (T)func(f);
    }
}

template <typename T>
GTable<T>::~GTable()
{
    delete[] values;
}

void* GWriteable::ReturnStuff()
{
    GFile* r;
    return (void*)r;
}