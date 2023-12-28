#pragma once

#include "common.h"

#define CHAX 1

#include "../configs.h"
#include "vanilla/_vanilla.h"

#include "debug-kit.h"

#define IS_IWRAM_PTR(ptr)    ((((u32)(ptr)) & 0xFF000000) == 0x03000000)
#define IS_EWRAM_PTR(ptr)    ((((u32)(ptr)) & 0xFF000000) == 0x02000000)
#define IS_ROM_PTR(ptr)      ((((u32)(ptr)) & 0xFF000000) == 0x08000000)
#define IS_ROM_THUMB(ptr)    ((((u32)(ptr)) & 0xFF000001) == 0x08000001)
#define IS_ROM_DATA(ptr)     ((((u32)(ptr)) & 0xFF000001) == 0x08000000)
#define IS_ANTI_HUFFMAN(ptr) ((((u32)(ptr)) & 0xFF000000) == 0x88000000)
