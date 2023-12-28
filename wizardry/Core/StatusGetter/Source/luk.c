#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_LUK(struct Unit * unit)
{
    int status = unit->lck;
    return msg_calc(status, unit, LukGetters);
}

int LukGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemLckBonus(GetUnitEquippedWeapon(unit));
}
