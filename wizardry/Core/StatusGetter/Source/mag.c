#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_MAG(struct Unit * unit)
{
    /* Todo */
    int status = unit->pow;
    return msg_calc(status, unit, MagGetters);
}

/* LynJump */
int GetUnitMagRange(struct Unit * unit)
{
    return 5 + MagGetter(unit) / 2;
}
