#pragma once

#include "common-chax.h"

int DecodeUtf8(const char * str, u32 * unicode_out, int * len);
int EncodeUtf8(u32 unicod, char * out, int * len);

void Sjis2Utf8(char * buf, int len);
