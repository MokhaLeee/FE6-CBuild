#include "common-chax.h"
#include "unit.h"
#include "item.h"
#include "map.h"
#include "mapwork.h"
#include "faction.h"
#include "playerphase.h"
#include "targetlist.h"
#include "trap.h"
#include "bmfx.h"
#include "itemuse.h"
#include "ai_unk.h"
#include "ai_decide.h"
#include "ai_battle.h"

#include "weapon-range.h"

bool AiEquipGetFlags(u16 * equip_flags_out)
{
    int i;

    u32 percent;

    if (GetUnitItemCount(gActiveUnit) == 0)
        return FALSE;

    for (i = 0; i < ITEMSLOT_INV_COUNT; ++i)
    {
        fu16 item;

        equip_flags_out[i] = 0;

        item = gActiveUnit->items[i];

        if (item == 0)
            break;

        if ((GetItemAttributes(item) & (ITEM_ATTR_WEAPON | ITEM_ATTR_STAFF)) == 0)
            continue;

        if ((GetItemAttributes(item) & ITEM_ATTR_LOCK_DRAGON) != 0)
            continue;

        if (!CanUnitUseWeapon(gActiveUnit, item) && !CanUnitUseStaff(gActiveUnit, item))
            continue;

        if (GetItemAttributes(item) & ITEM_ATTR_WEAPON)
        {
            if (NewGetItemMinRange(item, gActiveUnit) > 1)
                equip_flags_out[i] |= AI_EQUIP_FLAG_RANGE;

            if (NewGetItemMaxRange(item, gActiveUnit) == 1)
                equip_flags_out[i] |= AI_EQUIP_FLAG_MELEE;

            percent = Div(percent = GetItemUses(item) * 100, GetItemMaxUses(item));

            if (percent <= 10)
                equip_flags_out[i] |= AI_EQUIP_FLAG_LOW_USES;
        }
        else
        {
            AiUpdateUnitFlagsFromStaff(item);
            equip_flags_out[i] |= AI_EQUIP_FLAG_STAFF;
        }

        equip_flags_out[i] |= AI_EQUIP_FLAG_MIGHT(GetItemMight(item));
    }

#ifdef BUGFIX
    return TRUE;
#endif
}

void AiEquipGetDanger(int x, int y, u16 * range_danger_out, u16 * melee_danger_out, u16 * combined_danger_out)
{
    int iy, ix;
    int might;
    u16 item;

    *combined_danger_out = 0;
    *melee_danger_out = 0;
    *range_danger_out = 0;

    MapFill(gMapOther, 0);

    FOR_UNITS_ALL(unit,
    {
        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_RESCUED)) != 0)
            continue;

        if (AreUnitIdsAllied(gActiveUnitId, unit->id))
            continue;

        if (!AiIsWithinFlyingDistance(unit, x, y))
            continue;

        MapFlood_UpToMove(unit);

        if (gMapMovement[y][x] == 0xFF)
            continue;

        might = AiGetUnitAttackAndWeapon(unit, &item);

        if (NewGetItemMinRange(item, unit) > 1)
            *range_danger_out += might;

        if (NewGetItemMaxRange(item, unit) == 1)
            *melee_danger_out += might;

        for (iy = gMapSize.y - 1; iy >= 0; iy--)
        {
            for (ix = gMapSize.x - 1; ix >= 0; ix--)
            {
                if (gMapMovement[iy][ix] > MAP_MOVEMENT_MAX)
                    continue;

                if (gMapOther[iy][ix] + might <= UINT8_MAX)
                    gMapOther[iy][ix] = gMapOther[iy][ix] + might;
                else
                    gMapOther[iy][ix] = UINT8_MAX;
            }
        }
    })

    *combined_danger_out = *range_danger_out + *melee_danger_out;
}

bool AiTryCombatAt(int x, int y)
{
    int ix, iy;
    struct Unit * unit;

    fu16 item = GetUnitEquippedWeapon(gActiveUnit);

    if (item == 0)
        return FALSE;

    MapFill(gMapMovement, 0);
    MapIncInBoundedRange(x, y, NewGetItemMinRange(item, gActiveUnit), NewGetItemMaxRange(item, gActiveUnit));

    for (iy = gMapSize.y - 1; iy >= 0; iy--)
    {
        for (ix = gMapSize.x - 1; ix >= 0; ix--)
        {
            if (gMapMovementSigned[iy][ix] == 0)
                continue;

            if (gMapUnit[iy][ix] == 0)
                continue;

            if (AreUnitIdsAllied(gActiveUnitId, gMapUnit[iy][ix]) == 1)
                continue;

            unit = GetUnit(gMapUnit[iy][ix]);

            if (AiGetInRangeCombatPositionScoreComponent(x, y, unit))
            {
                AiSetDecision(x, y, AI_ACTION_COMBAT, unit->id, GetUnitEquippedWeaponSlot(gActiveUnit), 0, 0);
                return TRUE;
            }
        }
    }

    return FALSE;
}

void AiFillReversedAttackRangeMap(struct Unit * unit, u16 item)
{
    // reverse as in "this is the range this unit can be attacked *from*"

    MapFill(gMapRange, 0);
    MapIncInBoundedRange(unit->x, unit->y,
            NewGetItemMinRange(item, gActiveUnit),
            NewGetItemMaxRange(item, gActiveUnit));
}

void AiMakeMoveRangeMapsForUnitAndWeapon(struct Unit * unit, u16 item)
{
    int ix, iy;

    MapFlood_UpToMove(unit);
    MapFill(gMapRange, 0);

    for (iy = gMapSize.y-1; iy >= 0; iy--)
    {
        for (ix = gMapSize.x-1; ix >= 0; ix--)
        {
            if (gMapMovement[iy][ix] > MAP_MOVEMENT_MAX)
                continue;

            MapIncInBoundedRange(ix, iy, NewGetItemMinRange(item, unit), NewGetItemMaxRange(item, unit));
        }
    }
}

void AiMakeMoveRangeMapsForUnitAndWeapon2(struct Unit * unit, u16 item)
{
    int ix, iy;

    MapFlood_UpToMove(unit);
    MapFill(gMapRange, 0);

    for (iy = gMapSize.y-1; iy >= 0; iy--)
    {
        for (ix = gMapSize.x-1; ix >= 0; ix--)
        {
            if (gMapMovement[iy][ix] > MAP_MOVEMENT_MAX)
                continue;

            MapIncInBoundedRange(ix, iy, NewGetItemMinRange(item, unit), NewGetItemMaxRange(item, unit));
        }
    }
}

fu8 UnitActionMenu_Attack_SwitchIn(struct MenuProc * menu, struct MenuEntProc * ent)
{
    int reach;

    MapFill(gMapMovement, -1);
    MapFill(gMapRange, 0);

    reach = GetUnitWeaponReach(gActiveUnit, -1);
    BuildUnitStandingRangeForReach(gActiveUnit, reach);

    if (UNIT_ATTRIBUTES(gActiveUnit) & UNIT_ATTR_BALLISTA)
    {
        int item = GetBallistaItemAt(gActiveUnit->x, gActiveUnit->y);

        if (item != 0)
            MapIncInBoundedRange(gActiveUnit->x, gActiveUnit->y,
                NewGetItemMinRange(item, gActiveUnit), NewGetItemMaxRange(item, gActiveUnit));
    }

    StartLimitView(LIMITVIEW_BLUE | LIMITVIEW_RED);

    return 0;
}

fu8 UnitAttackBallista_Entry_SwitchIn(struct MenuProc * menu, struct MenuEntProc * ent)
{
    int item;

    MapFill(gMapMovement, -1);
    MapFill(gMapRange, 0);

    SetWorkingMap(gMapRange);

    item = GetBallistaItemAt(gActiveUnit->x, gActiveUnit->y);
    UpdateEquipInfoWindow(item);

    MapIncInBoundedRange(gActiveUnit->x, gActiveUnit->y,
        NewGetItemMinRange(item, gActiveUnit), NewGetItemMaxRange(item, gActiveUnit));
    StartLimitView(LIMITVIEW_RED);

    return 0;
}

void BuildUnitCompleteAttackRange(struct Unit * unit)
{
    int ix, iy;

    u32 mask = GetUnitWeaponReach(unit, -1);
    MapFill(gMapRange, 0);

    for (iy = 0; iy < gMapSize.y; iy++)
    {
        for (ix = 0; ix < gMapSize.x; ix++)
        {
            if (gMapMovement[iy][ix] > MAP_MOVEMENT_MAX)
                continue;

            if (gMapUnit[iy][ix])
                continue;

            AddMap(ix, iy, mask, 1, 0);
        }
    }
    SetWorkingMap(gMapMovement);
}

void ListAttackTargetsForWeapon(struct Unit * unit, int item)
{
    int x = unit->x;
    int y = unit->y;

    gSubjectUnit = unit;

    BeginTargetList(x, y);

    MapFill(gMapRange, 0);
    MapIncInBoundedRange(x, y,
        NewGetItemMinRange(item, unit),
        NewGetItemMaxRange(item, unit));

    ForEachUnitInRange(TryEnlistAttackUnitTarget);
    EnlistAttackObstacleTargets();
}

void func_fe6_08021278(struct Unit * unit)
{
    int item;

    int x = unit->x;
    int y = unit->y;

    gSubjectUnit = unit;

    BeginTargetList(x, y);

    item = GetDummyBallistaItemAt(x, y);

    if (item != 0)
    {
        MapFill(gMapRange, 0);
        MapIncInBoundedRange(x, y,
            NewGetItemMinRange(item, unit),
            NewGetItemMaxRange(item, unit));

        ForEachUnitInRange(func_fe6_08021240);
    }
}

// int GetUnitStaffReach(struct Unit * unit)

void BuildUnitCompleteStaffRange(struct Unit * unit)
{
    int i;
    MapFill(gMapRange, 0);

    for (i = 0; i < ITEMSLOT_INV_COUNT; i++)
    {
        u16 item = unit->items[i];
        if (!item)
            break;

        if (CanUnitUseStaff(unit, item))
            AddMapForItem(unit, item);
    }
}

void func_fe6_08030994(struct Unit * unit, u16 item)
{
    int flag = 0;

    if ((GetItemAttributes(item) & ITEM_ATTR_WEAPON) && NewGetItemMaxRange(item, unit) > 1)
        flag = AI_UNIT_FLAG_6;

    switch (GetItemEffect(item))
    {

    case ITEM_EFFECT_02:
    case ITEM_EFFECT_03:
    case ITEM_EFFECT_04:
    case ITEM_EFFECT_05:
    case ITEM_EFFECT_06:
    case ITEM_EFFECT_24:
    case ITEM_EFFECT_25:
        flag |= AI_UNIT_FLAG_HEALER;
        break;

    }

    unit->ai_flags |= flag;
}

/* In InjectMovGetter */
#if 0
bool AiCouldReachByBirdsEyeDistance(struct Unit * unit, struct Unit * other, u16 item)
{
    int maxDistance = RECT_DISTANCE(unit->x, unit->y, other->x, other->y);

    if (maxDistance <= UNIT_MOV(unit) + UNIT_MOV(other) + NewGetItemMaxRange(item, unit))
        return TRUE;

    return FALSE;
}

bool AiReachesByBirdsEyeDistance(struct Unit * unit, struct Unit * other, u16 item)
{
    int maxDistance = RECT_DISTANCE(unit->x, unit->y, other->x, other->y);

    if (maxDistance <= UNIT_MOV(unit) + NewGetItemMaxRange(item, unit))
        return TRUE;

    return FALSE;
}
#endif

int GetUnitBattleAiScore(struct Unit * unit)
{
    int i, item;

    u8 rangedCnt = 0;
    u8 meleeCnt = 0;

    for (i = 0; (i < ITEMSLOT_INV_COUNT) && (item = unit->items[i]); ++i)
    {
        if (!CanUnitUseWeapon(unit, item) && !CanUnitUseStaff(unit, item))
            continue;

        if (GetItemAttributes(item) & ITEM_ATTR_STAFF)
        {
            return 72;
        }

        if (GetItemAttributes(item) & ITEM_ATTR_WEAPON)
        {
            int range = NewGetItemMaxRange(item, unit);

            if (range > 1)
                rangedCnt++;
            else
                meleeCnt++;
        }
    }

    if (rangedCnt != 0)
        return 40;

    if (meleeCnt != 0)
        return 20;

    return 87;
}

int GetUnitWeaponReach(struct Unit * unit, int item_slot)
{
    int i, item, result = 0;

    if (item_slot >= 0)
        return NewGetItemReach(unit->items[item_slot], unit);

    for (i = 0; (i < ITEMSLOT_INV_COUNT) && (item = unit->items[i]); ++i)
    {
        if (CanUnitUseWeapon(unit, item))
            result |= NewGetItemReach(item, unit);
    }

    return result;
}

int GetUnitItemUseReach(struct Unit * unit, int item_slot)
{
    int i;
    u32 mask;
    u16 item;

    switch (item_slot) {
    case 0 ... 4:
        item = unit->items[item_slot];
        if (!CanUnitUseItem(unit, item))
            return 0;

        return NewGetItemReach(item, unit);

    default:
        mask = 0;
        for (i = 0; i < 5; i++)
        {
            item = unit->items[i];
            if (!item || CanUnitUseItem(unit, item))
                continue;

            mask |= NewGetItemReach(item, unit);
        }
        return mask;
    }
}

void BuildUnitStandingRangeForReach(struct Unit * unit, int reach)
{
    MapFill(gMapRange, 0);
    AddMap(unit->x, unit->y, reach, 1, 0);
}
