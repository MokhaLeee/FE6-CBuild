#include "common-chax.h"
#include "unit.h"
#include "item.h"
#include "map.h"
#include "mapwork.h"

#include "status-getter.h"
#include "weapon-range.h"

extern const msg_func WeaponMaxRangeGtters[];

int NewGetItemMinRange(int item, struct Unit * unit)
{
    int status;

    status = GetIInfo(ITEM_IID(item))->encoded_range >> 4;

    /* I think here we introduce a judgement may be safe */
    if (IS_EWRAM_PTR(unit) && unit->pinfo)
        status = msg_calc(status, unit, NULL);

    return status;
}

int NewGetItemMaxRange(int item, struct Unit * unit)
{
    int status;

    status = GetIInfo(ITEM_IID(item))->encoded_range & 0xF;

#ifdef CHAX
    if (status == 0)
        status = GetUnitMagRange(unit);
#endif

    /* I think here we introduce a judgement may be safe */
    if (IS_EWRAM_PTR(unit) && unit->pinfo)
        status = msg_calc(status, unit, WeaponMaxRangeGtters);

    return status;
}

STATIC_DECLAR u32 GetRangeMask(int min, int max)
{
    int i;
    u32 mask = 0;

    for (i = 0; i < 32; i++)
    {
        if (i < min)
            continue;

        if (i > max)
            continue;

        mask |= 1 << i;
    }
    return mask;
}

u32 NewGetItemReach(u16 item, struct Unit * unit)
{
    return GetRangeMask(
        NewGetItemMinRange(item, unit),
        NewGetItemMaxRange(item, unit)
    );
}

bool NewCanItemReachDistance(int item, int range, struct Unit * unit)
{
    if (range > NewGetItemMaxRange(item, unit))
        return FALSE;

    if (range < NewGetItemMinRange(item, unit))
        return FALSE;

    return TRUE;
}

void AddMap(int x, int y, u32 mask, int on, int off)
{
    int ix, iy;

    int X1 = x - 32;
    int X2 = x + 32;
    int Y1 = y - 32;
    int Y2 = y + 32;

    LIMIT_AREA(X1, 0, gMapSize.x);
    LIMIT_AREA(X2, 0, gMapSize.x);
    LIMIT_AREA(Y1, 0, gMapSize.y);
    LIMIT_AREA(Y2, 0, gMapSize.y);

    for (iy = Y1; iy < Y2; iy++)
    {
        for (ix = X1; ix < X2; ix++)
        {
            int distance = RECT_DISTANCE(x, y, ix, iy);
            if (mask & (1 << distance))
                gWorkingMap[iy][ix] += on;
            else
                gWorkingMap[iy][ix] += off;
        }
    }
}

void AddMapForItem(struct Unit * unit, u16 item)
{
    int min = NewGetItemMinRange(item, unit);
    int max = NewGetItemMaxRange(item, unit);

    if (max == 0)
        max = GetUnitMagRange(unit);

    MapAddInRange(unit->x, unit->y, max, 1);

    if (min != 0)
        MapAddInRange(unit->x, unit->y, min - 1, -1);
}

void ForEachUnit(void (* func)(struct Unit *), u8 ** map, const int off)
{
    int x, y;

    if (gMapSize.x < 0 || gMapSize.y < 0)
        return;

    for (y = 0; y < gMapSize.y; y++)
    {
        for (x = 0; x < gMapSize.x; x++)
        {
            if (off == map[y][x])
                continue;

            if (gMapUnit[y][x] != 0)
                func(GetUnit(gMapUnit[y][x]));
        }
    }
}
