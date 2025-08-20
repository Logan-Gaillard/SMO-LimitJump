#include <exlaunch.hpp>
#include <string.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/IUseNerve.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"
#include "Library/Se/SeFunction.h"
#include "Player/PlayerInputFunction.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Controller/InputFunction.h"

#include "Player/PlayerActorHakoniwa.h"

#include "System/GameSystem.h"
#include "hook/trampoline.hpp"
#include "prim/seadSafeString.h"
#include "types.h"

#include "logger/SDLogger.hpp" //My own logger (Mainly maked for another repo) it print log on folder and by svc too
#include "PlayerStates/PlayerStateSwim/PlayerStateSwim.h" //is Mario Swiming

#include "util/modules.hpp"


//PlayerActorHakoniwaAction lastPlayerAction;
bool isSurfaceHakoniwa = false;
bool needPlayJumpSE = false;

u64 jumpRemaining = 10;

void updateJumpRemaining() {
    if (jumpRemaining > 0) {
        SDLogger::log("Il reste %i sauts", jumpRemaining);
        jumpRemaining--;
    }
}

HOOK_DEFINE_TRAMPOLINE(GameSystemInit){
    static void Callback(GameSystem *thisPtr) {
        Orig(thisPtr);
    }
};


HOOK_DEFINE_TRAMPOLINE(PlayerInputFunctionIsTriggerJump){
    static bool Callback(const al::LiveActor *actor, s32 port) {
        //SDLogger::log("playerDoing: %i", playerDoing.getDoing());
        if(al::isPadTriggerA(port) || al::isPadTriggerB(port)){
            if(jumpRemaining == 0){
                if(al::isInWater(actor) && !isSurfaceHakoniwa) {
                    return Orig(actor, port);
                }
                needPlayJumpSE = true;
                return false;
            }
        }        
        return Orig(actor, port);
    }
};

HOOK_DEFINE_TRAMPOLINE(PlayerActorHakoniwaControl){
    static void Callback(PlayerActorHakoniwa *thisPtr) {
        if(needPlayJumpSE) {
            SDLogger::log("Sond effect joué !");
            const sead::SafeString nameSe = "InvalidCapAction";
            al::startSe(thisPtr, nameSe);
            needPlayJumpSE = false;
        }
        isSurfaceHakoniwa = thisPtr->mStateSwim->isSurface();
        Orig(thisPtr);
    }
};

// HOOK_DEFINE_TRAMPOLINE(MsgRequestPlayerSpinJump){
//     static bool Callback(al::HitSensor* source, al::HitSensor* target, f32 f1) {
//         SDLogger::log("SPINJMP Source: %s, target: %s, float: %f, HitSensorType: %i", source->mName, target->mName, f1, source->mSensorType);
//         if( source->mSensorType == al::HitSensorType::Player ||
//             source->mSensorType == al::HitSensorType::PlayerFoot ||
//             source->mSensorType == al::HitSensorType::EnemyBody
//         ) return false;
//         return Orig(source, target, f1);
//     }
// };
/*
HOOK_DEFINE_TRAMPOLINE(PlayerActorHakoniwaJump){
    static void Callback(PlayerActorHakoniwa *thisPtr) {
        if(al::isFirstStep(thisPtr)){
            if(jumpRemaining > 0) {
                updateJumpRemaining();
                SDLogger::log("Il reste %i sauts", jumpRemaining);
            }else{
                if(jumpRemaining == 0) {
                    ListActionPlayer capturedPAHAction = lastPlayerAction.getAction();

                    if(capturedPAHAction == ListActionPlayer::Waiting){
                        //al::setNerve(thisPtr, &NrvPlayerActorHakoniwa.Wait);
                        thisPtr->setNerveOnGround();
                    }
                    else if(capturedPAHAction == ListActionPlayer::Swimming){
                        //al::setNerve(thisPtr, &NrvPlayerActorHakoniwa.Swim);
                        thisPtr->setNerveOnGround();
                    }
                    else if(capturedPAHAction == ListActionPlayer::Falling){
                        al::setNerve(thisPtr, &NrvPlayerActorHakoniwa.Fall);
                        al::addVelocityY(thisPtr, -8.0f);
                    }
                    else if(capturedPAHAction == ListActionPlayer::Running){
                        //al::setNerve(thisPtr, &NrvPlayerActorHakoniwa.Run);
                        thisPtr->setNerveOnGround();
                    } else {
                        //al::setNerve(thisPtr, &NrvPlayerActorHakoniwa.Wait);
                        thisPtr->setNerveOnGround();
                    }
                }
                return;
            }
        }
        Orig(thisPtr);
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(SetNerveHook){
    static void Callback(al::IUseNerve* user, al::Nerve* nerve){

        al::Nerve* jumpNerve = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D78948);
        if(nerve == jumpNerve) {
            if(jumpRemaining == 0){
                needPlayJumpSE = true;
                return Orig(user, const_cast<al::Nerve*>(user->getNerveKeeper()->getCurrentNerve()));
            }
            updateJumpRemaining();
        }
        return Orig(user, nerve);
    }
};


extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();

    GameSystemInit::InstallAtOffset(0x535850);

    // Jump capture
    //PlayerActorHakoniwaJump::InstallAtSymbol("_ZN19PlayerActorHakoniwa7exeJumpEv");

    PlayerActorHakoniwaControl::InstallAtSymbol("_ZN19PlayerActorHakoniwa7controlEv");
    PlayerInputFunctionIsTriggerJump::InstallAtSymbol("_ZN19PlayerInputFunction13isTriggerJumpEPKN2al9LiveActorEi");

    SetNerveHook::InstallAtSymbol("_ZN2al8setNerveEPNS_9IUseNerveEPKNS_5NerveE");

    //PlayerActorHakoniwaActionHook::setInstance(&lastPlayerAction);
    //PlayerActorHakoniwaActionHook::initHooks();

    SDLogger::instance().init();
};