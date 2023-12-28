#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_RES(struct Unit * unit)
{
    int status = unit->res;
    return msg_calc(status, unit, ResGetters);
}

int ResGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemResBonus(GetUnitEquippedWeapon(unit));
}
