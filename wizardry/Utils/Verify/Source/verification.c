#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "verification.h"

static void crc(u32 * crc_add, u32 * crc_xor, u32 value)
{
    *crc_add += value;
    *crc_xor ^= value;
}

static u32 crc_calc(struct Unit * unit)
{
    u32 value;
    u32 crc_add = 0;
    u32 crc_xor = 0;

    /**
     * uid, pid, jid, level, equipped weapon
     */
    value = (unit->level << 16) + (ITEM_IID(GetUnitEquippedWeapon(unit)) << 8) + (unit->id & 0xFF);
    crc(&crc_add, &crc_xor, value);
    crc(&crc_add, &crc_xor, (u32)unit->pinfo);
    crc(&crc_add, &crc_xor, (u32)unit->jinfo);

    return crc_add + crc_xor;
}

bool CheckCRC_CHAX(struct Unit * unit, u32 crc)
{
    return crc == crc_calc(unit);
}

void WriteCRC_CHAX(struct Unit * unit, u32 * crc)
{
    *crc = crc_calc(unit);
}
