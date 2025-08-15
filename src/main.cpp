#include <exlaunch.hpp>
#include "System/GameSystem.h"
#include "hook/trampoline.hpp"

HOOK_DEFINE_TRAMPOLINE(GameSystemInit){
    static void Callback(GameSystem *thisPtr) {
        Orig(thisPtr);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();

    GameSystemInit::InstallAtOffset(0x535850);
};