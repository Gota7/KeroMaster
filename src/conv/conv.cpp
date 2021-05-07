#include "conv.h"
#include <string>
#include <vector>

#include "jisConvData.h"

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
    return fromShiftJIS(in_str.c_str(), in_str.size());
}

std::string fromShiftJIS(const char* data, unsigned int length) {
    std::vector<char> chrs;
    struct codepoint cp = {0};
    chrs.reserve(length);

    auto encodeU8 = [&](unsigned short c) {
        cp.encode(c);
        for (int i = 0; i < cp.len; i++) {
            chrs.push_back(cp.points[i]);
        }
    };

    for (unsigned int i = 0; i < length; i++) {
        auto remaining = length - i;

        if (remaining >= 2) {
            auto b = (unsigned char) data[i];
            if (b >= 0x81 && b <= 0x9f 
                || b >= 0xe0 && b <= 0xef 
                || b >= 0xfa && b <= 0xfc) {
                unsigned int b2 = (unsigned char) data[++i];
                auto c = ((unsigned int)b) << 8 | b2;
                encodeU8(sjis2UnicodeConvData[c]);
            } else if (b >= 0xa0 && b <= 0xdf) {
                encodeU8((unsigned short)b + 0xfec0);
            } else {
                encodeU8((unsigned short)b);
            }
        } else {
            auto b = (unsigned char) data[i];
            encodeU8(b);
        }
    }

    return std::string(chrs.begin(), chrs.end());
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