#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_SPD(struct Unit * unit)
{
    int status = unit->spd;
    return msg_calc(status, unit, SpdGetters);
}

int SpdGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemSpdBonus(GetUnitEquippedWeapon(unit));
}
