#include <exlaunch.hpp>
#include <string.h>

#include "Layout/StageSceneLayout.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/IUseNerve.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"
#include "Library/Se/SeFunction.h"
#include "Player/PlayerInputFunction.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Controller/InputFunction.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Draw/SubCameraRenderer.h"

#include "Player/PlayerActorHakoniwa.h"
#include "Scene/StageScene.h"

#include "System/GameSystem.h"
#include "hook/trampoline.hpp"
#include "prim/seadSafeString.h"
#include "types.h"

#include "logger/SDLogger.hpp" //My own logger (Mainly maked for another repo) it print log on folder and by svc too
#include "PlayerStates/PlayerStateSwim/PlayerStateSwim.h" //is Mario Swiming

#include "util/modules.hpp"

#include "Layout/JumpCounter.h"
#include "JumpData.h"

JumpCounter* jumpCounter = nullptr;


//PlayerActorHakoniwaAction lastPlayerAction;
bool isSurfaceHakoniwa = false;
bool needPlayJumpSE = false;

void updateJumpRemaining() {
    s32 jumpRemain = JumpData::getJumpRemain();
    if (jumpRemain > 0) {
        SDLogger::log("Il reste %i sauts", jumpRemain - 1);
        JumpData::decreaseJumpRemain(1);
    }
    jumpCounter->tryUpdateCount();
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
            s32 jumpRemain = JumpData::getJumpRemain();
            if(jumpRemain == 0){
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
            s32 jumpRemain = JumpData::getJumpRemain();
            if(jumpRemain == 0){
                needPlayJumpSE = true;
                return Orig(user, const_cast<al::Nerve*>(user->getNerveKeeper()->getCurrentNerve()));
            }
            updateJumpRemaining();
        }
        return Orig(user, nerve);
    }
};

HOOK_DEFINE_TRAMPOLINE(ConstructStageSceneLayout){
    static void Callback(StageSceneLayout* thisPtr, const char* char1, const al::LayoutInitInfo& layoutInitInfo, const al::PlayerHolder* playerHolder, const al::SubCameraRenderer* subCameraRenderer){
        Orig(thisPtr, char1, layoutInitInfo, playerHolder, subCameraRenderer);

        SDLogger::log("Adding new layout...");
        jumpCounter = new JumpCounter("JumpCounter", layoutInitInfo);
        SDLogger::log("New layout added");
    }
};

HOOK_DEFINE_TRAMPOLINE(CoinCounterTryStart){
    static void Callback(CoinCounter* thisPtr){
        Orig(thisPtr);
        jumpCounter->tryStart();
    }
};

HOOK_DEFINE_TRAMPOLINE(CoinCounterTryEnd){
    static void Callback(CoinCounter* thisPtr){
        Orig(thisPtr);
        jumpCounter->tryEnd();
    }
};


extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();

    GameSystemInit::InstallAtOffset(0x535850);

    // Jump capture
    PlayerActorHakoniwaControl::InstallAtSymbol("_ZN19PlayerActorHakoniwa7controlEv");
    PlayerInputFunctionIsTriggerJump::InstallAtSymbol("_ZN19PlayerInputFunction13isTriggerJumpEPKN2al9LiveActorEi");

    SetNerveHook::InstallAtSymbol("_ZN2al8setNerveEPNS_9IUseNerveEPKNS_5NerveE");

    //Layout management
    ConstructStageSceneLayout::InstallAtSymbol("_ZN16StageSceneLayoutC1EPKcRKN2al14LayoutInitInfoEPKNS2_12PlayerHolderEPKNS2_17SubCameraRendererE");
    CoinCounterTryStart::InstallAtSymbol("_ZN11CoinCounter8tryStartEv");
    CoinCounterTryEnd::InstallAtSymbol("_ZN11CoinCounter6tryEndEv");


    SDLogger::instance().init();

    JumpData::instance();
    JumpData::readFromSave();
};