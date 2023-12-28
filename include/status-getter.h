#pragma once

#include "common-chax.h"
#include "unit.h"

typedef int (* msg_func)(int old, struct Unit *);
extern const msg_func MhpGetters[], StrGetters[], MagGetters[], SklGetters[], \
        SpdGetters[], LukGetters[], DefGetters[], ResGetters[], MovGetters[], ConGetters[];

static inline int msg_calc(int status, struct Unit * unit, const msg_func * getters)
{
    const msg_func * it = getters;
    while (*it)
        status = (*it++)(status, unit);

    return status;
}

/* API */
int MhpGetter(struct Unit * unit);
int StrGetter(struct Unit * unit);
int MagGetter(struct Unit * unit);
int SklGetter(struct Unit * unit);
int SpdGetter(struct Unit * unit);
int LukGetter(struct Unit * unit);
int DefGetter(struct Unit * unit);
int ResGetter(struct Unit * unit);
int MovGetter(struct Unit * unit);
int ConGetter(struct Unit * unit);
