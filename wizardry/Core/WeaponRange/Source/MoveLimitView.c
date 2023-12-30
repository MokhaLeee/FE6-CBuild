#include "common-chax.h"
#include "proc.h"
#include "hardware.h"
#include "playerphase.h"
#include "constants/videoalloc_global.h"

#include "weapon-range.h"

void LimitView_Loop(struct GenericProc * proc)
{
    int frame = (GetGameTime() / 2) & 0x1F;

    if (proc->unk4A & LIMITVIEW_BLUE)
        ApplyPaletteExt(Pal_LimitViewBlue + frame, 0x20 * BGPAL_LIMITVIEW + 2, 0x20);

    if (proc->unk4A & LIMITVIEW_RED)
        ApplyPaletteExt(Pal_LimitViewRed + frame, 0x20 * (BGPAL_LIMITVIEW + 1) + 2, 0x20);

    if (proc->unk4A & LIMITVIEW_GREEN)
        ApplyPaletteExt(Pal_LimitViewGreen + frame, 0x20 * (BGPAL_LIMITVIEW + 1) + 2, 0x20);

#ifdef CHAX
    if (proc->unk4A & MOVLIMITV_RMAP_BLUE)
        ApplyPaletteExt(Pal_LimitViewBlue + frame, 0x20 * (BGPAL_LIMITVIEW + 1) + 2, 0x20);

    if (proc->unk4A & MOVLIMITV_MMAP_RED)
        ApplyPaletteExt(Pal_LimitViewRed + frame, 0x20 * BGPAL_LIMITVIEW + 2, 0x20);

    if (proc->unk4A & MOVLIMITV_MMAP_GREEN)
        ApplyPaletteExt(Pal_LimitViewGreen + frame, 0x20 * BGPAL_LIMITVIEW + 2, 0x20);
#endif
}
