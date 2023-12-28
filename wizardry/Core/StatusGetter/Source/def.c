#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_DEF(struct Unit * unit)
{
    int status = unit->def;
    return msg_calc(status, unit, DefGetters);
}

int DefGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemDefBonus(GetUnitEquippedWeapon(unit));
}
