#include "common-chax.h"
#include "unit.h"
#include "action.h"
#include "map.h"
#include "text.h"
#include "util.h"
#include "icon.h"
#include "proc.h"
#include "playerphase.h"
#include "mapwork.h"
#include "support.h"
#include "item.h"
#include "ai.h"
#include "ai_utility.h"
#include "ai_decide.h"
#include "ai_unk.h"
#include "ai_order.h"
#include "ai_script.h"
#include "ai_data.h"
#include "ai_battle.h"
#include "battle.h"
#include "armfunc.h"
#include "hardware.h"
#include "faction.h"
#include "statscreen.h"
#include "constants/videoalloc_global.h"
#include "constants/terrains.h"

#include "status-getter.h"
#include "weapon-range.h"

bool CanActiveUnitStillMove(void)
{
    SHOULD_BE_STATIC i8 SHOULD_BE_CONST adjLut[] =
    {
        -1,  0,
         0, -1,
        +1,  0,
         0, +1,
    };

#ifdef CHAX
    int move = MovGetter(gActiveUnit) - gAction.move_count;
#else
    int move = UNIT_MOV(gActiveUnit) - gAction.move_count;
#endif

    int xUnit = gActiveUnit->x;
    int yUnit = gActiveUnit->y;

    int i;

    for (i = 0; i < 4; ++i)
    {
        int xLocal = xUnit + adjLut[i*2+0];
        int yLocal = yUnit + adjLut[i*2+1];

        int cost;

        if (gMapUnit[yLocal][xLocal] & FACTION_RED)
            continue;

        cost = gActiveUnit->jinfo->mov_table[gMapTerrain[yLocal][xLocal]];

        if ((cost < 0) || (cost > move))
            continue;

        return TRUE;
    }

    return FALSE;
}

void PutStatScreenPersonalInfoPage(void)
{
    Decompress(gUnk_08307D58, gBuf);
    TmApplyTsa(gUiTmScratchB, gBuf, TILEREF(BGCHR_WINDOWFRAME, BGPAL_WINDOWFRAME));

    PutStatScreenText(gStatScreenPersonalInfoLabelsInfo);

    // display strength/magic labels

    if (UnitKnowsMagic(gStatScreenSt.unit))
    {
        // magic
        PutDrawText(gStatScreenSt.text + STATSCREEN_TEXT_POW,
            gUiTmScratchA + TM_OFFSET(1, 1),
            TEXT_COLOR_SYSTEM_GOLD, 0, 0,
            TEXT("魔力", "Mag"));
    }
    else
    {
        // strength
        PutDrawText(gStatScreenSt.text + STATSCREEN_TEXT_POW,
            gUiTmScratchA + TM_OFFSET(1, 1),
            TEXT_COLOR_SYSTEM_GOLD, 4, 0,
            TEXT("力", "Str"));
    }

    // display strength/magic stat value
    PutStatScreenStatWithBar(0, 5, 1,
        gStatScreenSt.unit->pow,
        GetUnitPower(gStatScreenSt.unit),
        UNIT_POW_CAP(gStatScreenSt.unit));

    // display skill stat value
    PutStatScreenStatWithBar(1, 5, 3,
        gStatScreenSt.unit->flags & UNIT_FLAG_RESCUING
            ? gStatScreenSt.unit->skl / 2 : gStatScreenSt.unit->skl,
        GetUnitSkill(gStatScreenSt.unit),
        gStatScreenSt.unit->flags & UNIT_FLAG_RESCUING
            ? UNIT_SKL_CAP(gStatScreenSt.unit) / 2 : UNIT_SKL_CAP(gStatScreenSt.unit));

    // display speed stat value
    PutStatScreenStatWithBar(2, 5, 5,
        gStatScreenSt.unit->flags & UNIT_FLAG_RESCUING
            ? gStatScreenSt.unit->spd/2 : gStatScreenSt.unit->spd,
        GetUnitSpeed(gStatScreenSt.unit),
        gStatScreenSt.unit->flags & UNIT_FLAG_RESCUING
            ? UNIT_SPD_CAP(gStatScreenSt.unit) / 2 : UNIT_SPD_CAP(gStatScreenSt.unit));

    // display luck stat value
    PutStatScreenStatWithBar(3, 5, 7,
        gStatScreenSt.unit->lck,
        GetUnitLuck(gStatScreenSt.unit),
        UNIT_LCK_CAP(gStatScreenSt.unit));

    // display defense stat value
    PutStatScreenStatWithBar(4, 5, 9,
        gStatScreenSt.unit->def,
        GetUnitDefense(gStatScreenSt.unit),
        UNIT_DEF_CAP(gStatScreenSt.unit));

    // display resistance stat value
    PutStatScreenStatWithBar(5, 5, 11,
        gStatScreenSt.unit->res,
        GetUnitResistance(gStatScreenSt.unit),
        UNIT_RES_CAP(gStatScreenSt.unit));

    // display movement stat value
    PutStatScreenStatWithBar(6, 13, 1,
        UNIT_MOV_BASE(gStatScreenSt.unit),
#ifdef CHAX
        MovGetter(gStatScreenSt.unit),
#else
        UNIT_MOV(gStatScreenSt.unit),
#endif
        UNIT_MOV_CAP(gStatScreenSt.unit));

    // display constitution stat value
    PutStatScreenStatWithBar(7, 13, 3,
        UNIT_CON_BASE(gStatScreenSt.unit),
#ifdef CHAX
        ConGetter(gStatScreenSt.unit),
#else
        UNIT_CON(gStatScreenSt.unit),
#endif
        UNIT_CON_CAP(gStatScreenSt.unit));

    // display unit aid
    PutNumber(gUiTmScratchA + TM_OFFSET(13, 5), TEXT_COLOR_SYSTEM_BLUE,
        GetUnitAid(gStatScreenSt.unit));

    // display unit aid icon
    PutIcon(gUiTmScratchA + TM_OFFSET(14, 5),
        GetAidIconFromAttributes(UNIT_ATTRIBUTES(gStatScreenSt.unit)),
        TILEREF(0, BGPAL_ICONS + 1));

    // display unit rescue name
    Text_InsertDrawString(gStatScreenSt.text + STATSCREEN_TEXT_RESCUE,
        24, TEXT_COLOR_SYSTEM_BLUE,
        GetUnitRescueName(gStatScreenSt.unit));

    // display status name
    Text_InsertDrawString(gStatScreenSt.text + STATSCREEN_TEXT_STATUS,
        24, TEXT_COLOR_SYSTEM_BLUE,
        GetUnitStatusName(gStatScreenSt.unit));

    // display status turns

    if (gStatScreenSt.unit->status != UNIT_STATUS_NONE)
    {
        PutNumberSmall(gUiTmScratchA + TM_OFFSET(16, 11),
            TEXT_COLOR_SYSTEM_WHITE,
            gStatScreenSt.unit->status_duration);
    }

    // display affininity icon and name

    PutIcon(gUiTmScratchA + TM_OFFSET(12, 9),
        GetUnitAffinityIcon(gStatScreenSt.unit),
        TILEREF(0, BGPAL_ICONS + 1));

    Text_InsertDrawString(gStatScreenSt.text + STATSCREEN_TEXT_AFFINITY,
        40, TEXT_COLOR_SYSTEM_BLUE, GetAffinityName(gStatScreenSt.unit->pinfo->affinity));
}

void DisplayUnitActionRange(struct Unit * unit)
{
    int flags = LIMITVIEW_BLUE;

#ifdef CHAX
    MapFlood_UpTo(gActiveUnit, MovGetter(gActiveUnit) - gAction.move_count);
#else
    MapFlood_UpTo(gActiveUnit, UNIT_MOV(gActiveUnit) - gAction.move_count);
#endif

    if (!(gActiveUnit->flags & UNIT_FLAG_HAD_ACTION))
    {
        switch (func_fe6_08018258(gActiveUnit))
        {

        case 0:
            MapFill(gMapRange, 0);
            break;

        case UNIT_USEBIT_WEAPON | UNIT_USEBIT_STAFF:
            if (gBmSt.swap_action_range_count & 1)
            {
                BuildUnitCompleteStaffRange(gActiveUnit);
                flags = LIMITVIEW_GREEN | LIMITVIEW_BLUE;
            }
            else
            {
                BuildUnitCompleteAttackRange(gActiveUnit);
                flags = LIMITVIEW_RED | LIMITVIEW_BLUE;
            }

            break;

        case UNIT_USEBIT_STAFF:
            BuildUnitCompleteStaffRange(gActiveUnit);
            flags = LIMITVIEW_GREEN | LIMITVIEW_BLUE;

            break;

        case UNIT_USEBIT_WEAPON:
            BuildUnitCompleteAttackRange(gActiveUnit);
            flags = LIMITVIEW_RED | LIMITVIEW_BLUE;

            break;

        }
    }

    StartLimitView(flags);
}

bool PlayerPhase_AttemptReMove(ProcPtr proc)
{
    if (!(UNIT_ATTRIBUTES(gActiveUnit) & UNIT_ATTR_RE_MOVE))
        return FALSE;

    if (gActiveUnit->flags & (UNIT_FLAG_HAD_ACTION | UNIT_FLAG_DEAD))
        return FALSE;

    if (gAction.id == ACTION_COMBAT || gAction.id == ACTION_STAFF)
        return FALSE;

#ifdef CHAX
    if (MovGetter(gActiveUnit) <= gAction.move_count)
#else
    if (UNIT_MOV(gActiveUnit) <= gAction.move_count)
#endif
        return FALSE;

    if (!CanActiveUnitStillMove())
        return FALSE;

    MapFill(gMapRange, 0);

    UnitBeginReMoveAction(gActiveUnit);

    gActiveUnit->flags |= UNIT_FLAG_HAD_ACTION;
    gActiveUnit->flags &= ~UNIT_FLAG_TURN_ENDED;

    if (gPlaySt.vision != 0)
        Proc_Goto(proc, L_PLAYERPHASE_MAPFADE_MOVE);
    else
        Proc_Goto(proc, L_PLAYERPHASE_MOVE);

    return TRUE;
}

void InitMovePath(bool display_only)
{
    Decompress(Img_MovePath, OBJ_VRAM0 + CHR_SIZE*OBCHR_MOVEPATH);
    ApplyPalette(Pal_MovePath, 0x10 + OBPAL_MOVEPATH);

    if (!display_only)
    {
#ifdef CHAX
        gMovePath->initMove = MovGetter(gActiveUnit) - gAction.move_count;
#else
        gMovePath->initMove = UNIT_MOV(gActiveUnit) - gAction.move_count;
#endif
        CutMovePath(0);
        AddMovePathSquare(gActiveUnit->x, gActiveUnit->y);

        gMovePath->move[0] = gMovePath->initMove;

        SetMovePathLastCursorPosition(-1, -1);

        UpdateMovePath();
    }
}

void MapFlood_UpToMove(struct Unit * unit)
{
    SetWorkingMovTable(unit->jinfo->mov_table);
    SetWorkingMap(gMapMovement);

#ifdef CHAX
    BeginMapFlood(unit->x, unit->y, MovGetter(unit), unit->id);
#else
    BeginMapFlood(unit->x, unit->y, UNIT_MOV(unit), unit->id);
#endif
}

int func_fe6_0802B9DC(void)
{
#ifdef CHAX
    MapFlood_UpTo(gActiveUnit, MovGetter(gActiveUnit) - gAction.move_count);
#else
    MapFlood_UpTo(gActiveUnit, UNIT_MOV(gActiveUnit) - gAction.move_count);
#endif
    return func_fe6_0802B99C();
}

void InitBattleUnit(struct BattleUnit * bu, struct Unit * unit)
{
    if (!unit)
        return;

    bu->unit = *unit;

    bu->unit.max_hp = GetUnitMaxHp(unit);
    bu->unit.pow = GetUnitPower(unit);
    bu->unit.skl = GetUnitSkill(unit);
    bu->unit.spd = GetUnitSpeed(unit);
    bu->unit.def = GetUnitDefense(unit);
    bu->unit.lck = GetUnitLuck(unit);
    bu->unit.res = GetUnitResistance(unit);
#ifdef CHAX
    bu->unit.bonus_con = ConGetter(unit);
    bu->unit.bonus_mov = MovGetter(unit);
#else
    bu->unit.bonus_con = UNIT_CON(unit);
    bu->unit.bonus_mov = UNIT_MOV(unit);
#endif

    bu->previous_level = bu->unit.level;
    bu->previous_exp = bu->unit.exp;
    bu->previous_hp = bu->unit.hp;

    bu->output_status = 0xFF;

    bu->change_hp = 0;
    bu->change_pow = 0;
    bu->change_skl = 0;
    bu->change_spd = 0;
    bu->change_def = 0;
    bu->change_res = 0;
    bu->change_lck = 0;
    bu->change_con = 0;

    gBattleUnitA.wexp_gain = 0;
    gBattleUnitB.wexp_gain = 0;

    bu->advantage_bonus_hit = 0;
    bu->advantage_bonus_damage = 0;

    bu->dealt_damage = FALSE;

    gBattleUnitA.weapon_broke = FALSE;
    gBattleUnitB.weapon_broke = FALSE;

    gBattleUnitA.exp_gain = 0;
    gBattleUnitB.exp_gain = 0;
}

void func_fe6_080308B0(void)
{
    int i, j, item;

    gAiSt.best_blue_mov = 0;

    for (i = FACTION_BLUE + 1; i < FACTION_BLUE + 0x40; ++i)
    {
        u8 mov;

        struct Unit * unit = GetUnit(i);

        if (!unit)
            continue;

        if (!unit->pinfo)
            continue;

        if (unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD))
            continue;

#ifdef CHAX
        mov = MovGetter(unit);
#else
        mov = UNIT_MOV(unit);
#endif
        if (mov > gAiSt.best_blue_mov)
            gAiSt.best_blue_mov = mov;

        for (j = 0; (j < ITEMSLOT_INV_COUNT) && (item = unit->items[j]); ++j)
        {
            if (!CanUnitUseWeapon(unit, item) && !CanUnitUseStaff(unit, item))
                continue;

            if (GetItemAttributes(item) & ITEM_ATTR_MAGIC)
                unit->ai_flags |= AI_UNIT_FLAG_SEEK_HEALING;

            func_fe6_08030968(unit, item);
            func_fe6_08030994(unit, item);
        }

        func_fe6_080309E0(unit);
    }
}

void AiTryMoveTowards(short x, short y, u8 action, u8 maxDanger, u8 arg_4)
{
    short ix, iy;

    u8 bestRange;

    short x_out = 0;
    short y_out = 0;

    if (gActiveUnit->x == x && gActiveUnit->y == y)
    {
        AiSetDecision(gActiveUnit->x, gActiveUnit->y, action, 0, 0, 0, 0);
        return;
    }

    if (arg_4)
    {
        MapFloodRange_Unitless(x, y, gActiveUnit->jinfo->mov_table);
    }
    else
    {
        AiMapFloodRangeFrom(x, y, gActiveUnit);
    }

    MapFlood_UpToMove(gActiveUnit);

    bestRange = gMapRange[gActiveUnit->y][gActiveUnit->x];
    x_out = -1;

    for (iy = gMapSize.y-1; iy >= 0; --iy)
    {
        for (ix = gMapSize.x-1; ix >= 0; --ix)
        {
            if (gMapMovement[iy][ix] > MAP_MOVEMENT_MAX)
                continue;

            if (gMapUnit[iy][ix] != 0 && gMapUnit[iy][ix] != gActiveUnitId)
                continue;

            if (maxDanger == 0)
            {
#ifdef CHAX
                if (MovGetter(gActiveUnit) < gAiSt.best_blue_mov && gMapOther[iy][ix] != 0)
#else
                if (UNIT_MOV(gActiveUnit) < gAiSt.best_blue_mov && gMapOther[iy][ix] != 0)
#endif
                    continue;
            }

            if (!AiCheckDangerAt(ix, iy, maxDanger))
                continue;

            if (gMapRange[iy][ix] > bestRange)
                continue;

            bestRange = gMapRange[iy][ix];
            x_out = ix;
            y_out = iy;
        }
    }

    if (x_out >= 0)
    {
        AiSetDecision(x_out, y_out, action, 0, 0, 0, 0);
    }
}

bool AiFindClosestUnlockPosition(int flags, struct Vec2i * pos_out)
{
    struct Vec2i tmp;
    int ix, iy;

    u16 unk = 0;

    u8 bestDistance = UINT8_MAX;
    u8 count = 0;

    for (iy = gMapSize.y-1; iy >= 0; iy--)
    {
        for (ix = gMapSize.x-1; ix >= 0; ix--)
        {
            if (gMapRange[iy][ix] > MAP_MOVEMENT_MAX)
                continue;

            switch (gMapTerrain[iy][ix])
            {

            case TERRAIN_DOOR:
                count++;

                if (flags & AI_FINDPOS_FLAG_NO_DOORS)
                    continue;

                if (!AiFindBestAdjacentPositionByFunc(ix, iy, AiGetPositionRange, &tmp))
                    continue;

                break;

            case TERRAIN_CHEST:
                count++;

                if (flags & AI_FINDPOS_FLAG_NO_CHESTS)
                    continue;

                tmp.x = ix;
                tmp.y = iy;

#ifdef CHAX
                if (gMapMovement[iy][ix] <= MovGetter(gActiveUnit))
#else
                if (gMapMovement[iy][ix] <= UNIT_MOV(gActiveUnit))
#endif
                {
                    if (flags & AI_FINDPOS_FLAG_CHECK_ENEMY)
                    {
                        if (gMapUnit[tmp.y][tmp.x] != 0 && !AreUnitIdsAllied(gActiveUnit->id, gMapUnit[tmp.y][tmp.x]))
                            continue;
                    }

                    if (flags & AI_FINDPOS_FLAG_CHECK_NEARBY_ENEMIES)
                    {
                        if (AiCountNearbyEnemyUnits(tmp.x, tmp.y) != 0)
                            continue;
                    }

                    pos_out->x = tmp.x;
                    pos_out->y = tmp.y;

                    return TRUE;
                }

                break;

            default:
                continue;

            }

            if (flags & AI_FINDPOS_FLAG_CHECK_ENEMY)
            {
                if (gMapUnit[tmp.y][tmp.x] != 0 && !AreUnitIdsAllied(gActiveUnit->id, gMapUnit[tmp.y][tmp.x]))
                    continue;
            }

            if (flags & AI_FINDPOS_FLAG_CHECK_NEARBY_ENEMIES)
            {
                if (AiCountNearbyEnemyUnits(tmp.x, tmp.y) != 0)
                    continue;
            }

            if (bestDistance <= gMapRangeSigned[tmp.y][tmp.x])
                continue;

            pos_out->x = tmp.x;
            pos_out->y = tmp.y;
            bestDistance = gMapRange[tmp.y][tmp.x];
        }
    }

    if ((unk & 0x10000) == 0) // if (1); required to match
        gAiSt.cmd_result[1] = 1;

    if (count == 0)
        gAiSt.cmd_result[0] = 5;

    if (bestDistance != UINT8_MAX)
        return TRUE;

    return FALSE;
}

bool AiCouldReachByBirdsEyeDistance(struct Unit * unit, struct Unit * other, u16 item)
{
    int maxDistance = RECT_DISTANCE(unit->x, unit->y, other->x, other->y);

#ifdef CHAX
    if (maxDistance <= MovGetter(unit) + MovGetter(other) + NewGetItemMaxRange(item, unit))
#else
    if (maxDistance <= UNIT_MOV(unit) + UNIT_MOV(other) + GetItemMaxRange(item))
#endif
        return TRUE;

    return FALSE;
}

bool AiReachesByBirdsEyeDistance(struct Unit * unit, struct Unit * other, u16 item)
{
    int maxDistance = RECT_DISTANCE(unit->x, unit->y, other->x, other->y);

#ifdef CHAX
    if (maxDistance <= UNIT_MOV(unit) + GetItemMaxRange(item))
#else
    if (maxDistance <= MovGetter(unit) + NewGetItemMaxRange(item, unit))
#endif
        return TRUE;

    return FALSE;
}

int GetUnitAiScore(struct Unit * unit)
{
#ifdef CHAX
    int score = UNIT_MOV(unit);
#else
    int score = MovGetter(unit);
#endif

    u16 lead = GetUnitLeaderPid(unit);

    if (UNIT_ATTRIBUTES(unit) & UNIT_ATTR_REFRESHER)
        return score - 149;

    if (!(unit->ai_flags & AI_UNIT_FLAG_SEEK_HEALING))
    {
        score += lead << 8;

        if (UNIT_ATTRIBUTES(unit) & UNIT_ATTR_STEAL)
            return score + 60;

        if ((unit->pinfo->id == lead) || (UNIT_ATTRIBUTES(unit) & UNIT_ATTR_LORD))
            return score + 87;

        score = score + GetUnitBattleAiScore(unit);
    }

    return score;
}

void AiMapFloodRange_UpToMove(struct Unit * unit)
{
    SetWorkingMovTable(unit->jinfo->mov_table);
    SetWorkingMap(gMapRange);

#ifdef CHAX
    BeginMapFlood(unit->x, unit->y, UNIT_MOV(unit), unit->id);
#else
    BeginMapFlood(unit->x, unit->y, MovGetter(unit), unit->id);
#endif
}

void AiAttemptOffensiveStaff(int slot, bool (* is_enemy)(struct Unit * unit))
{
    struct Vec2i pos;

    fu8 level_max = 0;
    fu8 accuracy;

    int x_decide = 0;
    int y_decide = 0;
    int target_decide = 0;

    if ((gAiSt.flags & AI_FLAG_BERSERKED) != 0)
        return;

    AiStayAwareMapFlood_UpToMove(gActiveUnit);

    FOR_UNITS_ALL(unit,
    {
        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD)) != 0)
            continue;

        if ((gAiSt.flags & AI_FLAG_BERSERKED) == 0 && is_enemy != NULL)
        {
            if (is_enemy(unit) == FALSE)
                continue;
        }

        if (unit->status != UNIT_STATUS_NONE)
            continue;

        if (!AiIsWithinRectDistance(
            gActiveUnit->x, gActiveUnit->y,
            unit->x, unit->y,
#ifdef CHAX
            GetUnitMagRange(gActiveUnit) + UNIT_MOV(gActiveUnit)))
#else
            GetUnitMagRange(gActiveUnit) + MovGetter(gActiveUnit)))
#endif
        {
            continue;
        }

        if (!AiIsUnitArmed(unit))
            continue;

        accuracy = GetOffensiveStaffAccuracy(gActiveUnit, unit);

        if (accuracy >= 5 && accuracy + unit->level >= level_max)
        {
            MapFill(gMapRange, 0);
            MapAddInRange(unit->x, unit->y, GetUnitMagRange(gActiveUnit), 1);

            if (AiFindGoodMovePositionWithinRange(&pos))
            {
                // BUG: this should be 'level_max = unit->level + accuracy;'
                // this bug enables a known exploit where units late into the unit list are prioritized
                // for selecting berserk/sleep staff target
                level_max = unit->level;
                x_decide = pos.x;
                y_decide = pos.y;
                target_decide = unit->id;
            }
        }
    })

    if (level_max != 0)
    {
        AiSetDecision(x_decide, y_decide, AI_ACTION_STAFF, target_decide, slot, 0, 0);
    }
}

void AiAttemptSilenceStaff(int slot, bool (* is_enemy)(struct Unit * unit))
{
    struct Vec2i pos;

    fu8 score_max = 0;
    fu8 score;

    int x_decide = 0;
    int y_decide = 0;
    int target_decide = 0;

    if ((gAiSt.flags & AI_FLAG_BERSERKED) != 0)
        return;

    AiStayAwareMapFlood_UpToMove(gActiveUnit);

    FOR_UNITS_ALL(unit,
    {
        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD)) != 0)
            continue;

        if ((gAiSt.flags & AI_FLAG_BERSERKED) == 0 && is_enemy != NULL)
        {
            if (is_enemy(unit) == FALSE)
                continue;
        }

        if (unit->status == UNIT_STATUS_SILENCED)
            continue;

        if (!AiUnitHasAnyStaff(unit))
            continue;

        if (!AiIsWithinRectDistance(
            gActiveUnit->x, gActiveUnit->y,
            unit->x, unit->y,
#ifdef CHAX
            GetUnitMagRange(gActiveUnit) + UNIT_MOV(gActiveUnit)))
#else
            GetUnitMagRange(gActiveUnit) + MovGetter(gActiveUnit)))
#endif
        {
            continue;
        }

        if (!AiIsUnitArmed(unit))
            continue;

        score = AiGetSilenceStaffTargetScore(unit);

        if (score != 0 && score >= score_max)
        {
            MapFill(gMapRange, 0);
            MapAddInRange(unit->x, unit->y, GetUnitMagRange(gActiveUnit), 1);

            if (AiFindGoodMovePositionWithinRange(&pos))
            {
                score_max = score;
                x_decide = pos.x;
                y_decide = pos.y;
                target_decide = unit->id;
            }
        }
    })

    if (score_max != 0)
    {
        AiSetDecision(x_decide, y_decide, AI_ACTION_STAFF, target_decide, slot, 0, 0);
    }
}

void AiAttemptRangedHealStaff(int slot, bool (* is_enemy)(struct Unit * unit))
{
    struct Vec2i pos;

    // lower is better
    fu8 score_min = 100;
    fu8 score;

    int x_decide = -1;
    int y_decide = -1;
    int target_decide = 0;

    if ((gAiSt.flags & AI_FLAG_BERSERKED) != 0)
        return;

    AiStayAwareMapFlood_UpToMove(gActiveUnit);

    if (gAiSt.maximum_heal_percent != 0)
        score_min = gAiSt.maximum_heal_percent;

    FOR_UNITS_ALL(unit,
    {
        if (gMapUnit[unit->y][unit->x] == gActiveUnitId)
            continue;

        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD)) != 0)
            continue;

        if ((gAiSt.flags & AI_FLAG_BERSERKED) == 0 && is_enemy != NULL)
        {
            if (is_enemy(unit) == TRUE)
                continue;
        }

        if (gAiSt.maximum_heal_percent == 0 && (unit->ai_flags & AI_UNIT_FLAG_SEEK_HEALING) == 0)
            continue;

        if (!AiIsWithinRectDistance(
            gActiveUnit->x, gActiveUnit->y,
            unit->x, unit->y,
#ifdef CHAX
            GetUnitMagRange(gActiveUnit) + UNIT_MOV(gActiveUnit)))
#else
            GetUnitMagRange(gActiveUnit) + MovGetter(gActiveUnit)))
#endif
        {
            continue;
        }

        MapFill(gMapRange, 0);
        MapAddInRange(unit->x, unit->y, GetUnitMagRange(gActiveUnit), 1);

        if (!AiFindGoodMovePositionWithinRange(&pos))
            continue;

        score = Div(100 * GetUnitCurrentHp(unit), GetUnitMaxHp(unit));

        if (score <= score_min)
        {
            score_min = score;
            x_decide = pos.x;
            y_decide = pos.y;
            target_decide = gMapUnit[unit->y][unit->x];
        }
    })

    if (x_decide != -1)
    {
        AiSetDecision(x_decide, y_decide, AI_ACTION_STAFF, target_decide, slot, 0, 0);
    }
}

bool AiFunc_080334F4(void const * arg)
{
    struct UnkAiFuncArg_080334F4 const * cast = arg;

    fu16 item;
    int ix;
    int iy;
    int x_unk;
    int y_unk;
    fu16 move;
    int x_prev;
    int y_prev;

#ifdef CHAX
    move = MovGetter(gActiveUnit) * cast->unk_04;
#else
    move = UNIT_MOV(gActiveUnit) * cast->unk_04;
#endif

    move = move >> 4;

    item = GetUnitEquippedWeapon(gActiveUnit);

    x_unk = cast->unk_00[((gActiveUnit->ai_config & 0x1FC0) >> 8) * 2 + 0];
    y_unk = cast->unk_00[((gActiveUnit->ai_config & 0x1FC0) >> 8) * 2 + 1];

    x_prev = gActiveUnit->x;
    y_prev = gActiveUnit->y;

    gActiveUnit->x = x_unk;
    gActiveUnit->y = y_unk;

    if ((cast->unk_05 != 0) && (item != 0))
    {
        AiFloodMovementAndRange(gActiveUnit, move, item);

        if (gMapRangeSigned[y_unk][x_unk] == 0)
        {
            gActiveUnit->x = x_prev;
            gActiveUnit->y = y_prev;
            AiTryMoveTowards(x_unk, y_unk, 0, 0xff, 1);
            return TRUE;
        }
    }
    else
    {
        SetWorkingMovTable(gActiveUnit->jinfo->mov_table);

        SetWorkingMap(gMapRange);
        BeginMapFlood(gActiveUnit->x, gActiveUnit->y, move, 0);

        if (gMapRange[y_unk][x_unk] > MAP_MOVEMENT_MAX)
        {
            gActiveUnit->x = x_prev;
            gActiveUnit->y = y_prev;
            AiTryMoveTowards(x_unk, y_unk, 0, 0xff, 1);
            return TRUE;
        }
    }

    gActiveUnit->x = x_prev;
    gActiveUnit->y = y_prev;

    MapFlood_UpToMove(gActiveUnit);

    for (iy = gMapSize.y - 1; iy >= 0; iy--)
    {
        for (ix = gMapSize.x - 1; ix >= 0; ix--)
        {
            if (item != 0)
            {
                if ((gMapMovementSigned[iy][ix] < MAP_MOVEMENT_MAX) && (gMapRangeSigned[iy][ix] != 0))
                    continue;

                gMapMovementSigned[iy][ix] = -1;
            }
            else
            {
                if ((gMapMovementSigned[iy][ix] < MAP_MOVEMENT_MAX) && (gMapRangeSigned[iy][ix] < MAP_MOVEMENT_MAX))
                    continue;

                gMapMovementSigned[iy][ix] = -1;
            }
        }
    }

    AiAttemptCombatWithinMovement(AiIsUnitEnemy);

    if (gAiDecision.action_performed == TRUE)
        return TRUE;

    AiTryMoveTowards(x_unk, y_unk, 0, UINT8_MAX, 1);

    return TRUE;
}

bool AiFunc_CountEnemiesInRange(void const * arg)
{
    struct AiCountEnemiesInRangeArg const * cast = arg;

    fu16 item;
    int ix, iy;
    fu16 move;

    fu8 count = 0;

#ifdef CHAX
    move = (UNIT_MOV(gActiveUnit) * cast->move_coeff_q4);
#else
    move = (MovGetter(gActiveUnit) * cast->move_coeff_q4);
#endif

    move = move >> 4;

    item = GetUnitEquippedWeapon(gActiveUnit);

    if (cast->attack_range != 0 && item != 0)
    {
        AiFloodMovementAndRange(gActiveUnit, move, item);

        for (iy = gMapSize.y - 1; iy >= 0; iy--)
        {
            for (ix = gMapSize.x - 1; ix >= 0; ix--)
            {
                if (gMapRangeSigned[iy][ix] == 0)
                    continue;

                if (AiFunc_CountEnemiesInRange_IsEnemyAt(ix, iy) == TRUE)
                    count++;
            }
        }
    }
    else
    {
        SetWorkingMovTable(gActiveUnit->jinfo->mov_table);
        SetWorkingMap(gMapRange);

        BeginMapFlood(gActiveUnit->x, gActiveUnit->y, move, 0);

        for (iy = gMapSize.y - 1; iy >= 0; iy--)
        {
            for (ix = gMapSize.x - 1; ix >= 0; ix--)
            {
                if (gMapRange[iy][ix] > MAP_MOVEMENT_MAX)
                    continue;

                if (AiFunc_CountEnemiesInRange_IsEnemyAt(ix, iy) == TRUE)
                    count++;
            }
        }
    }

    gAiSt.cmd_result[cast->result_slot] = count;

    return 0;
}

bool AiIsWithinFlyingDistance(struct Unit * unit, int x, int y)
{
#ifdef CHAX
    int mov = UNIT_MOV(unit);
#else
    int mov = MovGetter(unit);
#endif
    int dist = RECT_DISTANCE(x, y, unit->x, unit->y);

    if (mov >= dist)
        return TRUE;

    return FALSE;
}

bool AiTryMoveTowardsEscape(void)
{
    struct AiEscapePt const * escape_pt;

    MapFlood_UpTo(gActiveUnit, MAP_MOVEMENT_EXTENDED);
    escape_pt = AiGetNearestEscapePoint();

    if (escape_pt != NULL)
    {
#ifdef CHAX
        if (gMapMovementSigned[escape_pt->y][escape_pt->x] <= UNIT_MOV(gActiveUnit))
#else
        if (gMapMovementSigned[escape_pt->y][escape_pt->x] <= MovGetter(gActiveUnit))
#endif
        {
            AiTryMoveTowards(escape_pt->x, escape_pt->y, AI_ACTION_NONE, -1, TRUE);

            AiSetDecision(gAiDecision.x_move, gAiDecision.y_move, AI_ACTION_ESCAPE,
                escape_pt->x, escape_pt->y, escape_pt->facing, 0);

            return TRUE;
        }
        else
        {
            AiTryMoveTowards(escape_pt->x, escape_pt->y, AI_ACTION_NONE, -1, FALSE);
            return gAiDecision.action_performed;
        }
    }

    return FALSE;
}
