#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_SKL(struct Unit * unit)
{
    int status = unit->skl;
    return msg_calc(status, unit, SklGetters);
}

int SklGetter_RescuingDebuff(int old, struct Unit * unit)
{
    if (unit->flags & UNIT_FLAG_RESCUING)
        old = old / 2;

    return old;
}

int SklGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemSklBonus(GetUnitEquippedWeapon(unit));
}
