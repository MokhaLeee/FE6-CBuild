#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_POW(struct Unit * unit)
{
    int status = unit->pow;
    return msg_calc(status, unit, StrGetters);
}

int StrGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemPowBonus(GetUnitEquippedWeapon(unit));
}
