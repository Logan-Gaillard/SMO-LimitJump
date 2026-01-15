#include "KaronWingJump.hpp"
#include <cstdint>
#include <sys/_intsup.h>
#include "Enemy/KaronWingStateHack.h"
#include "Player/HackerStateWingFly.h"
#include "JumpData.h"
#include "Player/PlayerHackKeeper.h"
#include "hook/trampoline.hpp"
#include "logger/SDLogger.hpp"
#include "Player/IUsePlayerHack.h"

const char* KaronWingJump::name = "KaronWing";
const int KaronWingJump::decreaseDuration = 60; //Very 60 frames

HOOK_DEFINE_TRAMPOLINE(KaronWingStateHackControl){
static void Callback(KaronWingStateHack* thisPtr){
        if(!thisPtr->mStateWingFly->isOnGround()){
            if(KaronWingJump::instance().cooldown > 0){
                KaronWingJump::instance().decreaseCooldown();
            }else{
                JumpData::updateJumpRemain();
                KaronWingJump::instance().resetCooldown();
            }
        }
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(KaronWingStateHackCaptureFly){
    static void Callback(KaronWingStateHack* thisPtr){
        SDLogger::log("In KaronWing Capture Fly Hack");
        int jumpRemain = JumpData::getJumpRemain();
        if(jumpRemain == 0){
            //Force end hack
            if(thisPtr->mStateWingFly == nullptr){
            }else{
                PlayerHackKeeper* playerHackKeeper = (*thisPtr->mPlayerHack)->getPlayerHackKeeper();
                playerHackKeeper->cancelHack();
                //Return void and don't continue the original function
                return;
            }
        }
        return Orig(thisPtr);
    }
};

void KaronWingJump::initHooks(){
    KaronWingStateHackCaptureFly::InstallAtSymbol("_ZN18KaronWingStateHack10exeWingFlyEv");
    KaronWingStateHackControl::InstallAtSymbol("_ZN18KaronWingStateHack7controlEv"); 
}

/*
As a short explanation:
Assembly doesn't know variable names, it just knows offsets. In the case of KuriboWingHackState, the game stores its HackerStateWingFly* at offset 0x28, so in your mod, you also need to access offset 0x28. This could also be done manually with a whole bunch of unsafe operations, like this:
HackerStateWingFly* state = (HackerStateWingFly*) (((u8*) kuriboWingHackState) + 0x28);
... but that's weird and unreadable. Crafting up structs helps with organization and readability, and it's basically just used as a list of "if I say X, access offset Y".
*/
