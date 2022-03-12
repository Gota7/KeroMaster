#include "gfile.h"
#include <memory.h>

// If the OS is macOS, define the bswap functions
#if __APPLE__
#include <libkern/OSByteOrder.h>
#define __bswap_16(x) OSSwapInt16(x)
#define __bswap_32(x) OSSwapInt32(x)
#define __bswap_64(x) OSSwapInt64(x)
#endif

#ifdef _WIN32
#define __bswap_16(x) _byteswap_ushort(x)
#define __bswap_32(x) _byteswap_ulong(x)
#define __bswap_64(x) _byteswap_uint64(x)
#endif

u32 GFile::systemEndian = 0;
enum e_endian
{
    EndianBig,
    EndianLittle
};

GFile::GFile()
{
    isMem = true;
    Init();
}

GFile::GFile(const char *fileName)
{
    if (!FileExists(fileName))
    {
        CreateFile(fileName);
    }
    handle = fopen(fileName, "r+");
    this->fileName = fileName;
    isMem = false;
    Init();
}

void GFile::Init()
{
    if (systemEndian == 0)
    {
        union
        {
            u8 c[4];
            u32 i;
        } u;
        u.i = 0x01020304;
        if (0x04 == u.c[0])
        {
            systemEndian = EndianLittle;
        }
        else
        {
            systemEndian = EndianBig;
        }
    }
    if (systemEndian == EndianLittle)
    {
        swapEndian = false;
    }
    else
    {
        swapEndian = true;
    }
    if (isMem)
    {
        fileSize = 0;
    }
    else
    {
        fseek(handle, 0, SEEK_END);
        fileSize = ftell(handle);
    }
    position = 0;
}

void GFile::SetEndian(bool big)
{
    if (big && systemEndian != EndianBig)
    {
        swapEndian = true;
    }
    else
    {
        swapEndian = false;
    }
}

void GFile::Close()
{
    if (!isMem)
    {
        fclose(handle);
    }
}

u64 GFile::Size()
{
    return fileSize;
}

void GFile::ToMem()
{
    u64 oldPos = position;
    position = 0;
    buff.resize(fileSize);
    Read(&buff[0], fileSize);
    fclose(handle);
    isMem = true;
    position = oldPos;
}

void GFile::ToFile(const char *fileName)
{
    isMem = false;
    CreateFile(fileName);
    handle = fopen(fileName, "r+");
    u64 oldPos = position;
    position = 0;
    Write(&buff[0], fileSize);
    position = oldPos;
    buff.clear();
}

void GFile::DumpData(void *dest)
{
    if (isMem)
    {
        memcpy(dest, &buff[0], fileSize);
    }
    else
    {
        u64 oldPos = position;
        position = 0;
        Read(dest, fileSize);
        position = oldPos;
    }
}

void GFile::DumpData(const char *fileName)
{
    FILE *tmp = fopen(fileName, "w");
    if (isMem)
    {
        fwrite(&buff[0], sizeof(u8), fileSize, tmp);
    }
    else
    {
        u8 *tmpBuff = (u8 *)malloc(fileSize);
        u64 oldPos = position;
        position = 0;
        Read(tmpBuff, fileSize);
        fwrite(&tmpBuff, sizeof(u8), fileSize, tmp);
        position = oldPos;
        free(tmpBuff);
    }
    fclose(tmp);
}

void GFile::Align(size_t size)
{
    while (position % size != 0)
    {
        if (position == fileSize)
        {
            Write((u8)0);
        }
        else
        {
            position++;
        }
    }
}

void GFile::Clear()
{
    if (isMem)
    {
        buff.clear();
    }
    else
    {
        fclose(handle);
        handle = fopen(fileName.c_str(), "w+");
    }
}

bool GFile::FileExists(const char *fileName)
{
    FILE *file;
    if ((file = fopen(fileName, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}

void GFile::CreateFile(const char *fileName)
{
    FILE *tmp = fopen(fileName, "w");
    fclose(tmp);
}

size_t GFile::Read(void *dest, size_t size)
{
    if (size > fileSize - position)
    {
        size = fileSize - position;
    }
    if (isMem)
    {
        memcpy(dest, &buff[position], size);
    }
    else
    {
        fseek(handle, position, SEEK_SET);
        fread(dest, size, sizeof(u8), handle);
    }
    position += size;
    return size;
}

s8 GFile::ReadS8()
{
    s8 tmp;
    Read(&tmp, 1);
    return tmp;
}

u8 GFile::ReadU8()
{
    u8 tmp;
    Read(&tmp, 1);
    return tmp;
}

s16 GFile::ReadS16()
{
    s16 tmp;
    Read(&tmp, 2);
    if (swapEndian)
    {
        tmp = __bswap_16(tmp);
    }
    return tmp;
}

u16 GFile::ReadU16()
{
    u16 tmp;
    Read(&tmp, 2);
    if (swapEndian)
    {
        tmp = __bswap_16(tmp);
    }
    return tmp;
}

s32 GFile::ReadS32()
{
    s32 tmp;
    Read(&tmp, 4);
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    return tmp;
}

u32 GFile::ReadU32()
{
    u32 tmp;
    Read(&tmp, 4);
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    return tmp;
}

s64 GFile::ReadS64()
{
    s64 tmp;
    Read(&tmp, 8);
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    return tmp;
}

u64 GFile::ReadU64()
{
    u64 tmp;
    Read(&tmp, 8);
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    return tmp;
}

f32 GFile::ReadF32()
{
    f32 tmp;
    Read(&tmp, 4);
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    return tmp;
}

f64 GFile::ReadF64()
{
    f64 tmp;
    Read(&tmp, 8);
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    return tmp;
}

std::string GFile::ReadStr()
{
    std::string ret = "";
    char c = ReadU8();
    while (c != 0)
    {
        ret += c;
        c = ReadU8();
    }
    return ret;
}

std::string GFile::ReadStrFixed(u32 len)
{
    std::string ret = "";
    for (u32 i = 0; i < len; i++)
    {
        ret += ReadU8();
    }
    return ret;
}

void GFile::Write(void *src, size_t size)
{
    if (isMem)
    {
        if (position + size > fileSize)
        {
            buff.reserve(fileSize);
        }
        memcpy(&buff[position], src, size);
    }
    else
    {
        fseek(handle, position, SEEK_SET);
        fwrite(src, size, sizeof(u8), handle);
    }
    fileSize = fileSize - position + size;
    position += size;
}

void GFile::Write(s8 val)
{
    Write(&val, 1);
}

void GFile::Write(u8 val)
{
    Write(&val, 1);
}

void GFile::Write(s16 val)
{
    s16 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_16(tmp);
    }
    Write(&tmp, 2);
}

void GFile::Write(u16 val)
{
    u16 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_16(tmp);
    }
    Write(&tmp, 2);
}

void GFile::Write(s32 val)
{
    s32 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    Write(&tmp, 4);
}

void GFile::Write(u32 val)
{
    u32 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    Write(&tmp, 4);
}

void GFile::Write(s64 val)
{
    s64 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    Write(&tmp, 8);
}

void GFile::Write(u64 val)
{
    u64 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    Write(&tmp, 8);
}

void GFile::Write(f32 val)
{
    f32 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_32(tmp);
    }
    Write(&tmp, 4);
}

void GFile::Write(f64 val)
{
    f64 tmp = val;
    if (swapEndian)
    {
        tmp = __bswap_64(tmp);
    }
    Write(&tmp, 8);
}

void GFile::Write(std::string val)
{
    for (u32 i = 0; i < val.length(); i++)
    {
        Write(&val[i], 1);
    }
}

void GFile::WriteNullTerminated(std::string val)
{
    Write(val);
    Write((u8)0);
}