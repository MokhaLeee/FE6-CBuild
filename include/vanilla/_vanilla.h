#pragma once

#include "common.h"
#include "proc.h"
#include "item.h"
#include "gamecontroller.h"
#include "statscreen.h"

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

/* statscreen.c */
extern struct StatScreenTextInfo const gStatScreenPersonalInfoLabelsInfo[];

/* movepath.c */
enum { MOVEPATH_MAX = 20 };

enum
{
    MOVEPATH_DIR_NONE,
    MOVEPATH_DIR_RIGHT,
    MOVEPATH_DIR_DOWN,
    MOVEPATH_DIR_LEFT,
    MOVEPATH_DIR_UP,
};

struct MovePath
{
#if !BUGFIX
    // This is just unused/padding, but a proc header happen to fit here perfectly
    /* 00 */ PROC_HEADER;
#endif

    /* 29 */ i8 xLast;
    /* 2A */ i8 yLast;
    /* 2B */ u8 initMove;
    /* 2C */ i8 path;
    /* 2D */ i8 x[MOVEPATH_MAX];
    /* 41 */ i8 y[MOVEPATH_MAX];
    /* 55 */ i8 move[MOVEPATH_MAX];
};

void SetMovePathLastCursorPosition(u16 x, u16 y);
void CutMovePath(i8 path);
void AddMovePathSquare(i8 x, i8 y);
int FindMovePathSquare(i8 x, i8 y);
void MovePathFloodFromTail(void);
void RebuildMovePath(void);
bool MovePathIsValid(void);
void UpdateMovePath(void);
u8 GetMovePathDirFrom(u8 path);
u8 GetMovePathDirTo(u8 path);
bool ShouldDrawMovePathBitAt(short x, short y, u8 xEdge, u8 yEdge);
void DrawMovePath(void);

extern struct MovePath * CONST_DATA gMovePath;

/* code_0802B814.c */
int func_fe6_0802B99C(void);

/* item.c */
struct IInfo const * GetIInfo(int iid);
