#pragma once

#include "common-chax.h"
#include "unit.h"

struct ChaxUnitListHeader {
    i8 uid;
    u8 pid, jid;
    u8 level;
    u8 weapon;

    u8 _pad_[3];
};

bool CheckHeader_CHAX(struct ChaxUnitListHeader * header, struct Unit * unit);
void WriteHeader_CHAX(struct ChaxUnitListHeader * header, struct Unit * unit);

bool CheckCRC_CHAX(void const * data, int size, u32 crc);
void WriteCRC_CHAX(void const * data, int size, u32 * crc);
