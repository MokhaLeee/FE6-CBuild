#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_CON(struct Unit * unit)
{
    int status = UNIT_CON(unit);
    return msg_calc(status, unit, ConGetters);
}
