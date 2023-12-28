#include "common-chax.h"
#include "unit.h"
#include "item.h"

#include "status-getter.h"

int MSG_MHP(struct Unit * unit)
{
    int status = unit->max_hp;
    return msg_calc(status, unit, MhpGetters);
}

int MhpGetter_ItemBonus(int old, struct Unit * unit)
{
    return old + GetItemHpBonus(GetUnitEquippedWeapon(unit));
}

int StatusGetterMinusZero(int old, struct Unit * unit)
{
    if (old < 0)
        old = 0;

    return old;
}

/* LynJump */
int GetUnitCurrentHp(struct Unit * unit)
{
    if (unit->hp > GetUnitMaxHp(unit))
        unit->hp = GetUnitMaxHp(unit);

    return unit->hp;
}

/* LynJump */
void SetUnitHp(struct Unit * unit, int hp)
{
    unit->hp = hp;

    if (unit->hp > GetUnitMaxHp(unit))
        unit->hp = GetUnitMaxHp(unit);
}

/* LynJump */
void AddUnitHp(struct Unit * unit, int amount)
{
    int hp = unit->hp;

    hp += amount;

    if (hp > GetUnitMaxHp(unit))
        hp = GetUnitMaxHp(unit);

    if (hp < 0)
        hp = 0;

    unit->hp = hp;
}
