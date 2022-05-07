#include "conv.h"

#include "jisConvData.h"
#include "iconv.h"

#include <stdexcept>
#include <string>
#include <vector>

struct codepoint {
    unsigned char points[4];
    int len;

    void encode(unsigned short codepoint);
};

void codepoint::encode(unsigned short codepoint) {
    if (codepoint <= 0x7f) {
        this->points[0] = (unsigned char) codepoint;
        this->len = 1;
    } else if (codepoint <= 0x7ff) {
        this->points[0] = 0xc0 | (unsigned char) (codepoint >> 6);
        this->points[1] = 0x80 | (unsigned char) (codepoint & 0x3f);
        this->len = 2;
    } else {
        this->points[0] = 0xe0 | (unsigned char) (codepoint >> 12);
        this->points[1] = 0x80 | (unsigned char) ((codepoint >> 6) & 0x3f);
        this->points[2] = 0x80 | (unsigned char) (codepoint & 0x3f);
        this->len = 3;
    }

    this->points[this->len] = 0;
}

static unsigned short decodeCodePoint(const char** in_text, const char* in_text_end) {
    if (*in_text >= in_text_end) return 0;

    const char* p = *in_text;

    unsigned int remaining = (unsigned int) (in_text_end - p);
    unsigned char b = (unsigned char) *p++;

    if (b <= 0x7f) {
        *in_text = p;
        return (unsigned short) b;
    } else if (b <= 0xdf && remaining >= 2) {
        unsigned char b2 = (unsigned char) *p++;

        *in_text = p;
        return ((unsigned short)b & 0x1f) << 6 | ((unsigned short)b2 & 0x3f);
    } else if (b <= 0xef && remaining >= 3) {
        unsigned char b2 = (unsigned char) *p++;
        unsigned char b3 = (unsigned char) *p++;

        unsigned int cp = ((unsigned int)b & 0x0f) << 12 | ((unsigned int)b2 & 0x3f) << 6 | ((unsigned int)b3 & 0x3f);

        *in_text = p;
        return (cp >= 0xffff) ? 0xfffd : (unsigned short)cp;
    } else {
        if (remaining >= 4) {
            p += 4;
        } else {
            p += remaining;
        }

        *in_text = p;
        return 0xfffd;
    }
}

std::string fromShiftJIS(std::string& in_str) {
    // use iconv to convert from shift-jis to utf-8
    iconv_t cd = iconv_open("UTF-8", "SHIFT-JIS");
    if (cd == (iconv_t)-1)
    {
        throw std::runtime_error("iconv_open failed");
    }

    char *out_buf = new char[in_str.size() * 4];
    size_t out_size = in_str.size() * 4;
    char *out_ptr = out_buf;
    size_t in_size = in_str.size();
    char *in_ptr = (char *)in_str.c_str();
    size_t r = iconv(cd, &in_ptr, &in_size, &out_ptr, &out_size);

    if (r == (size_t)-1)
    {
        delete[] out_buf;
        iconv_close(cd);
        throw std::runtime_error("iconv failed");
    }

    std::string result(out_buf, out_buf + (in_str.size() * 4 - out_size));
    delete[] out_buf;
    iconv_close(cd);
    return result;
}

std::string toShiftJIS(std::string& in_str) {
    return toShiftJIS(in_str.data(), in_str.size());
}

std::string toShiftJIS(const char* data, unsigned int length) {
    std::vector<char> chrs;
    chrs.reserve(length);

    const char* ptr = data;
    const char* end = data + length;

    while (ptr < end) {
        auto unicode = decodeCodePoint(&ptr, end);
        auto sjiscode = unicode2SjisConvData[unicode];

        if (sjiscode <= 0xff) {
            chrs.push_back((char) sjiscode);
        } else {
            chrs.push_back((char) (sjiscode >> 8));
            chrs.push_back((char) (sjiscode & 0xff));
        }
    }

    return std::string(chrs.begin(), chrs.end());
}