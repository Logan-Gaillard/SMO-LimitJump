#include "KuriboWingJump.hpp"
#include "JumpData.h"
#include "PlayerStates/KuriboWingHackState/KuriboWingHackState.h"
#include "Player/HackerStateWingFly.h"
#include "hook/trampoline.hpp"

const char* KuriboWingJump::name = "KuriboWing";
const int KuriboWingJump::decreaseDuration = 30; //Very 30 frames

HOOK_DEFINE_TRAMPOLINE(KuriboWingHackStateControl){
    static void Callback(KuriboWingHackState* thisPtr){
        if(!thisPtr->mHackerStateWingFly->isOnGround()){
            int jumpRemain = JumpData::getJumpRemain();
            if(jumpRemain <= 0){
                return;
            }
            if(KuriboWingJump::instance().cooldown > 0){
                KuriboWingJump::instance().decreaseCooldown();
            }else{
                JumpData::updateJumpRemain(true, 1);
                KuriboWingJump::instance().resetCooldown();
            }
        }
        return Orig(thisPtr);
    }
};

void KuriboWingJump::initHooks(){
    KuriboWingHackStateControl::InstallAtSymbol("_ZN19KuriboWingHackState7controlEv"); 
}