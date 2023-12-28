#pragma once

#include "common-chax.h"
#include "unit.h"
#include "item.h"

bool CheckCRC_CHAX(struct Unit * unit, u32 crc);
void WriteCRC_CHAX(struct Unit * unit, u32 * crc);
