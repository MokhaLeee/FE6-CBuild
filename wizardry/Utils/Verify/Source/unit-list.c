#include "common-chax.h"
#include "unit.h"
#include "item.h"
#include "verification.h"

bool CheckHeader_CHAX(struct ChaxUnitListHeader * header, struct Unit * unit)
{
    if (header->uid != unit->id)
        return FALSE;

    if (header->pid != UNIT_PID(unit))
        return FALSE;

    if (header->jid != UNIT_JID(unit))
        return FALSE;

    if (header->level != unit->level)
        return FALSE;

    if (header->weapon != GetUnitEquippedWeapon(unit))
        return FALSE;

    return TRUE;
}

void WriteHeader_CHAX(struct ChaxUnitListHeader * header, struct Unit * unit)
{
    header->uid = unit->id;
    header->pid = UNIT_PID(unit);
    header->jid = UNIT_JID(unit);
    header->level = unit->level;
    header->weapon = GetUnitEquippedWeapon(unit);
}
