struct FaceInfo;
#include "common-chax.h"
#include "gamecontroller.h"
#include "bm.h"
#include "hardware.h"

typedef int (* const hook_func)();
extern hook_func GameInitHooks[];

void StartGame(void)
{
    hook_func * it;
    struct GameController * proc;

    SetMainFunc(OnMain);
    SetOnVBlank(OnVBlank);

    /* External modular */
    for (it = GameInitHooks; *it; it++)
        (*it)();

    proc = SpawnProc(ProcScr_GameController, PROC_TREE_3);

    proc->next_action = GAME_ACTION_0;
    proc->next_chapter = 0;

    proc->demo_counter = 0;
}
