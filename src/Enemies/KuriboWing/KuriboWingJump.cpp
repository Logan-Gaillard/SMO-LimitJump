#include "KuriboWingJump.hpp"
#include "Enemies/KuriboWing/KuriboWing.h"
#include "JumpData.h"
#include "Player/HackerStateWingFly.h"
#include "./KuriboWingHackState.h"
#include "Player/IUsePlayerHack.h"
#include "Player/PlayerHackKeeper.h"
#include "Util/Hack.h"
#include "hook/trampoline.hpp"
#include "logger/SDLogger.hpp"

const char* KuriboWingJump::name = "KuriboWing";
const int KuriboWingJump::decreaseDuration = 60; //Very 60 frames (1 second)

HOOK_DEFINE_TRAMPOLINE(KuriboWingHackStateControl){
    static void Callback(KuriboWingHackState* thisPtr){
        if(!thisPtr->mHackerStateWingFly->isOnGround()){
            if(KuriboWingJump::instance().cooldown > 0){
                KuriboWingJump::instance().decreaseCooldown();
            }else{
                JumpData::updateJumpRemain();
                KuriboWingJump::instance().resetCooldown();
            }
        }
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(KuriboWingCaptureFly){
    static void Callback(KuriboWing* thisPtr){
        int jumpRemain = JumpData::getJumpRemain();
        if(jumpRemain == 0){
            //Force end hack
            if(thisPtr->mPlayerHack == nullptr){
            }else{
                PlayerHackKeeper* playerHackKeeper = thisPtr->mPlayerHack->getPlayerHackKeeper();
                playerHackKeeper->cancelHack();
                //Return void and don't continue the original function
                return;
            }
        }
        return Orig(thisPtr);
    }
};

void KuriboWingJump::initHooks(){
    KuriboWingHackStateControl::InstallAtSymbol("_ZN19KuriboWingHackState7controlEv"); 
    KuriboWingCaptureFly::InstallAtSymbol("_ZN10KuriboWing13exeCaptureFlyEv");
}