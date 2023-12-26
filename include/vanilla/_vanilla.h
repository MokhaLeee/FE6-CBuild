#pragma once

#include "common.h"
#include "proc.h"
#include "gamecontroller.h"

/* gamecontroller.c */
struct GameController
{
    /* 00 */ PROC_HEADER;

    /* 29 */ u8 next_action;
    /* 2A */ u8 next_chapter;
    /* 2B */ u8 demo_counter;
    /* 2C */ u8 previous_demo_class_set;
    /* 2E */ i16 clock;
};

bool GC_StartClassDemo(struct GameController * proc);
void GC_CheckSramResetKeyCombo(struct GameController * proc);
void GC_InitSramResetScreen(struct GameController * proc);
void GC_InitFastStartCheck(struct GameController * proc);
void GC_FastStartCheck(struct GameController * proc);
void GC_PostIntro(struct GameController * proc);
void GC_PostDemo(struct GameController * proc);
void GC_PostMainMenu(struct GameController * proc);
void GC_InitTutorial(struct GameController * proc);
void GC_InitTrialChapter(struct GameController * proc);
void GC_ClearSuspend(struct GameController * proc);
void GC_PostChapter(struct GameController * proc);
void GC_CheckForGameEnded(struct GameController * proc);
void GC_PostLoadSuspend(struct GameController * proc);
void GC_InitNextChapter(struct GameController * proc);
void GC_InitDemo(struct GameController * proc);
void GC_DarkenScreen(struct GameController * proc);

/* ramfunc.c */
extern u8 RamFuncArea[];

extern void (* DrawGlyphRamFunc)(u16 const * cvtLut, void * chr, u32 const * glyph, int offset);
extern void (* DecodeStringRamFunc)(char const * src, char * dst);
extern void (* PutOamHiRamFunc)(int x, int y, u16 const * oam_list, int oam2);
extern void (* PutOamLoRamFunc)(int x, int y, u16 const * oam_list, int oam2);
extern void (* MapFloodCoreStepRamFunc)();
extern void (* MapFloodCoreRamFunc)(void);

/* msg.c */
extern u8 const * const gMsgTable[];
extern char sMsgString[0x1000];
extern int sActiveMsg;
