#include "common-chax.h"
#include "unit.h"
#include "text.h"

#include "status-getter.h"

void DrawUnitConText(struct Text * text, struct Unit * unit)
{
    ClearText(text);

    Text_InsertDrawString(text, 0, TEXT_COLOR_SYSTEM_GOLD, TEXT("体格", "Con"));
    Text_InsertDrawNumberOrBlank(text, 56, TEXT_COLOR_SYSTEM_BLUE, ConGetter(unit));
}

int GetUnitAid(struct Unit * unit)
{
    if (!(UNIT_ATTRIBUTES(unit) & UNIT_ATTR_MOUNTED))
        return ConGetter(unit) - 1;

    if (UNIT_ATTRIBUTES(unit) & UNIT_ATTR_FEMALE)
        return 20 - ConGetter(unit);

    return 25 - ConGetter(unit);
}

bool CanUnitCarry(struct Unit * unit, struct Unit * other)
{
    int aid = GetUnitAid(unit);
    int con = ConGetter(other);

    return (aid >= con) ? TRUE : FALSE;
}
