#include "common-chax.h"
#include "decoding.h"

static int GetChLenUtf8(const char * str)
{
    const u8 * utf8_in = (const u8 *)str;
    u8 cod = utf8_in[0];

    if ((0b11111000 & cod) == 0b11110000)
        return 4;

    if ((0b11110000 & cod) == 0b11100000)
        return 3;

    if ((0b11100000 & cod) == 0b11000000)
        return 2;

    if ((0b10000000 & cod) == 0x0)
        return 1;

    LogErrorf("Failed on decoding at %#X!", str);
    return -1;
}

/* UTF8 ===> Unicode */
int DecodeUtf8(const char * str, u32 * unicode_out, int * len)
{
    u32 unicod;
    const u8 * utf8_in = (const u8 *)str;

    switch (GetChLenUtf8(str)) {
    case 1:
        *unicode_out = *utf8_in;
        *len = 1;
        return 0;

    case 2:
        unicod = utf8_in[0] & 0b00011111;
        unicod = (unicod << 0x6) | (utf8_in[1] & 0b00111111);

        *unicode_out = unicod;
        *len = 2;
        return 0;

    case 3:
        unicod = utf8_in[0] & 0b00001111;
        unicod = (unicod << 0x6) | (utf8_in[1] & 0b00111111);
        unicod = (unicod << 0x6) | (utf8_in[2] & 0b00111111);

        *unicode_out = unicod;
        *len = 3;
        return 0;

    case 4:
        unicod = utf8_in[0] & 0b00000111;
        unicod = (unicod << 0x6) | (utf8_in[1] & 0b00111111);
        unicod = (unicod << 0x6) | (utf8_in[2] & 0b00111111);
        unicod = (unicod << 0x6) | (utf8_in[3] & 0b00111111);

        *unicode_out = unicod;
        *len = 4;
        return 0;

    default:

        LogErrorf("Failed on decoding at %#X!", str);
        *unicode_out = 0;
        *len = 0;
        return -1;
    }
}

/* Unicode ===> UTF8 */
int EncodeUtf8(u32 unicod, char * out, int * len)
{
    switch (unicod) {
    case 0x00 ... 0x7F:
        out[0] = unicod;
        *len = 1;
        return 0;

    case 0x80 ... 0x7FF:
        out[0] = 0xC0 + ((unicod) >> 6);
        out[1] = 0x80 + ((unicod) & 0x3F);
        *len = 2;
        return 0;

    case 0x800 ... 0xFFFF:
        out[0] = 0xE0 + ((unicod >> 12));
        out[1] = 0x80 + ((unicod >> 6) & 0x3F);
        out[2] = 0x80 + ((unicod) & 0x3F);
        *len = 3;
        return 0;

    default:
        out[0] = 0xF0 + ((unicod >> 18));
        out[1] = 0x80 + ((unicod >> 12) & 0x3F);
        out[2] = 0x80 + ((unicod >> 6) & 0x3F);
        out[3] = 0x80 + ((unicod) & 0x3F);
        *len = 4;
        return 0;
    }
}
