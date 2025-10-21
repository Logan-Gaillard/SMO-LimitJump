#include <exlaunch.hpp>
#include <string.h>

#include "Layout/StageSceneLayout.h"
#include "Library/Controller/JoyPadAccelPoseAnalyzer.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/IUseNerve.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"
#include "Library/Se/SeFunction.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Controller/InputFunction.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Draw/SubCameraRenderer.h"

#include "Util/PlayerUtil.h"
#include "Util/Hack.h"

#include "Player/HackerStateWingFly.h"
#include "Player/PlayerInputFunction.h"
#include "Player/PlayerActorHakoniwa.h"
#include "Player/PlayerInput.h"
#include "Scene/StageScene.h"
#include "Util/DemoUtil.h"

#include "System/GameSystem.h"
#include "hook/trampoline.hpp"
#include "prim/seadSafeString.h"
#include "types.h"

#include "logger/SDLogger.hpp" //My own logger (Mainly maked for another repo) it print log on folder and by svc too
#include "PlayerStates/PlayerStateSwim/PlayerStateSwim.h" //is Mario Swiming
#include "PlayerStates/KuriboWingHackState/KuriboWingHackState.h"

#include "util/modules.hpp"

#include "Layout/JumpCounter.h"
#include "JumpData.h"

JumpCounter* jumpCounter = nullptr; //Jump remain layout
bool isHakoniwaDemo = false;        //If Mario can't move (demo mode)
bool isSurfaceHakoniwa = false;     // If Mario is on the surface on water or not
bool needPlayJumpSE = false;        // If we need to play Sound Effect for not having enough jumps
bool isPlayerHack = false;
int kuriboWingCooldown = 60;        // Kuribo Wing cooldown to increase jump very 60 frames if is in the air

void updateJumpRemaining(bool decrease, int amount) {
    if (decrease) {
        if(JumpData::getJumpRemain() > 0)
            JumpData::decreaseJumpRemain(amount);
    } else {
        JumpData::increaseJumpRemain(amount);
    }
    int jumpRemain = JumpData::getJumpRemain();
    SDLogger::log("Il reste %i sauts", jumpRemain);
    jumpCounter->tryUpdateCount();
}

HOOK_DEFINE_TRAMPOLINE(GameSystemInit){
    static void Callback(GameSystem *thisPtr) {
        Orig(thisPtr);
    }
};


HOOK_DEFINE_TRAMPOLINE(PlayerInputFunctionIsTriggerJump){
    static bool Callback(const al::LiveActor *actor, int port) {
        //SDLogger::log("playerDoing: %i", playerDoing.getDoing());
        if(isHakoniwaDemo)
            return Orig(actor, port);

        if((al::isPadTriggerA(port) || al::isPadTriggerB(port)) && PlayerInputFunction::isTriggerTalk(actor, port) == false) {
            int jumpRemain = JumpData::getJumpRemain();
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

HOOK_DEFINE_TRAMPOLINE(JoyPadAccelPoseAnalyzerIsSwingAnyHand){
    static bool Callback(al::JoyPadAccelPoseAnalyzer* thisPtr) {
        int jumpRemain = JumpData::getJumpRemain();
        bool result = Orig(thisPtr);
        //SDLogger::log("Résultat: %s, jumps: %i", result ? "true" : "false", jumpRemain);

        if(result && jumpRemain <= 0 && isPlayerHack) {
            //needPlayJumpSE = true;
            return false;
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(PlayerActorHakoniwaControl){
    static void Callback(PlayerActorHakoniwa *thisPtr) {
        isHakoniwaDemo = rs::isActiveDemo(thisPtr);

        if(!isHakoniwaDemo) {
            if(al::isPadTriggerUp())
                updateJumpRemaining(false, 2);
        
            if(needPlayJumpSE) {
                jumpCounter->startNoAuthAnim();
                const sead::SafeString nameSe = "InvalidCapAction";
                al::startSe(thisPtr, nameSe);
                needPlayJumpSE = false;
            }

            isSurfaceHakoniwa = thisPtr->mStateSwim->isSurface();
            isPlayerHack = rs::isPlayerHack(thisPtr);
        }
        Orig(thisPtr);
    }
};

bool isJumpingNerve(al::Nerve* nerve){
    // All jump nerves
    //PlayerActorHakoniwa
    al::Nerve* pahJumpNerve = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D78948);
    al::Nerve* pahLongJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D78988); 
    //PlayerStateNormalWallJump
    al::Nerve* psnwjWallJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D7E998);
    //PlayerStateHeadSliding
    al::Nerve* pshsdive = (al::Nerve*) exl::util::modules::GetTargetOffset(0x001D7E118);
    //PlayerStateLongJump
    al::Nerve* psljJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D7E6D8);

    //KuriboStateHack
    al::Nerve* kshJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA38);
    al::Nerve* kshHigh = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA40);
    al::Nerve* kshTurnJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA48);
    al::Nerve* kshJumpBurn = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA50);

    //FrogStateHack
    al::Nerve* fshJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58660);
    al::Nerve* fshHighJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58668);
    al::Nerve* fshTrampleJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58670);


    if(
        nerve == pahJumpNerve ||
        nerve == pahLongJump ||

        nerve == psnwjWallJump ||
        nerve == pshsdive ||

        nerve == psljJump ||

        nerve == kshJump ||
        nerve == kshHigh ||
        nerve == kshTurnJump ||
        nerve == kshJumpBurn ||

        nerve == fshJump ||
        nerve == fshHighJump ||
        nerve == fshTrampleJump
    ){
        return true;
    }

    return false;
}

HOOK_DEFINE_TRAMPOLINE(SetNerveHook){
    static void Callback(al::IUseNerve* user, al::Nerve* nerve){
        //SDLogger::log("Nerve changed to %x", nerve);
        if(isJumpingNerve(nerve)) {
            int jumpRemain = JumpData::getJumpRemain();

            if(jumpRemain <= 0){
                needPlayJumpSE = true;
                if(isJumpingNerve(const_cast<al::Nerve*>(user->getNerveKeeper()->getCurrentNerve()))){
                    return;
                }else{
                    return Orig(user, const_cast<al::Nerve*>(user->getNerveKeeper()->getCurrentNerve()));
                }
            }

            updateJumpRemaining(true, 1);
        }
        return Orig(user, nerve);
    }
};

HOOK_DEFINE_TRAMPOLINE(KuriboWingHackStateControl){
    static void Callback(KuriboWingHackState* thisPtr){
        if(!thisPtr->mHackerStateWingFly->isOnGround()){
            int jumpRemain = JumpData::getJumpRemain();
            if(jumpRemain <= 0){
                if(thisPtr->mHackerStateWingFly->mHacker){
                    SDLogger::log("End Hack");
                    rs::endHack((IUsePlayerHack**) &thisPtr->mHackerStateWingFly->mHacker);
                }
                return;
            }
            if(kuriboWingCooldown > 0){
                kuriboWingCooldown--;
            }else{
                updateJumpRemaining(true, 1);
                kuriboWingCooldown = 60;
            }
        }
        return Orig(thisPtr);
    }
};


//-------------
// LAYOUT PART
//-------------
HOOK_DEFINE_TRAMPOLINE(ConstructStageSceneLayout){
    static void Callback(StageSceneLayout* thisPtr, const char* char1, const al::LayoutInitInfo& layoutInitInfo, const al::PlayerHolder* playerHolder, const al::SubCameraRenderer* subCameraRenderer){
        Orig(thisPtr, char1, layoutInitInfo, playerHolder, subCameraRenderer);
        jumpCounter = new JumpCounter("JumpCounter", layoutInitInfo);
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
    JoyPadAccelPoseAnalyzerIsSwingAnyHand::InstallAtSymbol("_ZNK2al23JoyPadAccelPoseAnalyzer14isSwingAnyHandEv");

    SetNerveHook::InstallAtSymbol("_ZN2al8setNerveEPNS_9IUseNerveEPKNS_5NerveE");

    // Layout Part
    ConstructStageSceneLayout::InstallAtSymbol("_ZN16StageSceneLayoutC1EPKcRKN2al14LayoutInitInfoEPKNS2_12PlayerHolderEPKNS2_17SubCameraRendererE");
    CoinCounterTryStart::InstallAtSymbol("_ZN11CoinCounter8tryStartEv");
    CoinCounterTryEnd::InstallAtSymbol("_ZN11CoinCounter6tryEndEv");

    KuriboWingHackStateControl::InstallAtSymbol("_ZN19KuriboWingHackState7controlEv"); 


    SDLogger::instance().init();

    JumpData::instance();
    JumpData::readFromSave();
};