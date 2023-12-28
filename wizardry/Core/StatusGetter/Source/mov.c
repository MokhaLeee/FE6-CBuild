#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_MOV(struct Unit * unit)
{
    int status = UNIT_MOV(unit);
    return msg_calc(status, unit, MovGetters);
}
