#include <exlaunch.hpp>
#include <string.h>

#include "Enemies/Pukupuku/PukupukuJump.hpp"
#include "Enemies/KaronWing/KaronWingJump.hpp"
#include "Enemies/KuriboWing/KuriboWingJump.hpp"
#include "Layout/StageSceneLayout.h"
#include "Library/Controller/JoyPadAccelPoseAnalyzer.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/IUseNerve.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"
#include "Library/Nerve/NerveAction.h"
#include "Library/Nerve/NerveActionCtrl.h"
#include "Library/Se/SeFunction.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Controller/InputFunction.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Draw/SubCameraRenderer.h"

#include "Player/PlayerHackKeeper.h"
#include "Sequence/ChangeStageInfo.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolderAccessor.h"
#include "Util/PlayerUtil.h"
#include "Util/Hack.h"

#include "Save/SaveJump.h"
#include "Player/HackerStateWingFly.h"
#include "Player/PlayerInputFunction.h"
#include "Player/PlayerActorHakoniwa.h"
#include "Player/IUsePlayerCollision.h"
#include "Player/PlayerInput.h"
#include "Scene/StageScene.h"
#include "Util/DemoUtil.h"

#include "System/GameSystem.h"
#include "hook/trampoline.hpp"
#include "prim/seadSafeString.h"

#include "logger/SDLogger.hpp" //My own logger (Mainly maked for another repo) it print log on folder and by svc too
#include "PlayerStates/PlayerStateSwim/PlayerStateSwim.h" //is Mario Swiming

#include "util/modules.hpp"

#include "Layout/JumpCounter.h"
#include "JumpData.h"

JumpCounter* jumpCounter = nullptr; //Jump remain layout
bool isHakoniwaDemo = false;        //If Mario can't move (demo mode)
bool isSurfaceHakoniwa = false;     // If Mario is on the surface on water or not
bool needPlayJumpSE = false;        // If we need to play Sound Effect for not having enough jumps
bool isPlayerHack = false;          // if the player is currently in capture
bool playerCanSwing = true;         // if the player can do swing motion

HOOK_DEFINE_TRAMPOLINE(GameSystemInit){
    static void Callback(GameSystem *thisPtr) {
        Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(StageSceneControl){
    static void Callback(StageScene* thisPtr) {
        if(al::isPadTriggerRight()){
            GameDataHolderAccessor gameDataHolderAccessor(thisPtr->mSceneObjHolder);
            ChangeStageInfo changeStageInfo = ChangeStageInfo(gameDataHolderAccessor.mData, "", (const char*)"MoonWorldCaptureParadeStage", false, -1, ChangeStageInfo::SubScenarioType::NO_SUB_SCENARIO);
            GameDataFunction::tryChangeNextStage(gameDataHolderAccessor.mData, &changeStageInfo);
        }
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
        if((result && jumpRemain <= 0 && isPlayerHack) && !playerCanSwing) {
            //Play sound effect
            // TODO : Make a cooldown for soudEffect
            return false;
        }
        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(RSIsCollisionCodeJump){
    static bool Callback(const IUsePlayerCollision* collision) {
        int jumpRemain = JumpData::getJumpRemain();
        if(jumpRemain == 0){
            return false;
        }
        return Orig(collision);
    }
};

HOOK_DEFINE_TRAMPOLINE(PlayerActorHakoniwaControl){
    static void Callback(PlayerActorHakoniwa *thisPtr) {
        isHakoniwaDemo = rs::isActiveDemo(thisPtr);

        const char* nerveActionName  = thisPtr->getNerveKeeper()->getActionCtrl()->getAction(0)->getActionName();
        SDLogger::log("Current nerve action: %s", nerveActionName);

        if(!isHakoniwaDemo) {
            if(al::isPadTriggerUp())
                JumpData::updateJumpRemain(false, 2);
        
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
    //al::Nerve* psljJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D7E6D8);

    //KuriboStateHack
    al::Nerve* kshJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA38);
    al::Nerve* kshHigh = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA40);
    al::Nerve* kshTurnJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA48);
    al::Nerve* kshJumpBurn = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C9EA50);

    //FrogStateHack
    al::Nerve* fshJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58660);
    al::Nerve* fshHighJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58668);
    al::Nerve* fshTrampleJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D58670);

    //Pukupuku (Fish)
    al::Nerve* captureJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01CA6050);
    al::Nerve* captureCaptureJumpOut = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01CA60E8);

    //Motorcycle
    al::Nerve* motorcycleJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D17058);
    al::Nerve* motorcycleRideRunJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D17078);
    al::Nerve* motorcycleRideWaitJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01D17060);

    //Koopa (Bowser)
    al::Nerve* koopaJumpStart = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C6B470);

    //Koopa2D
    al::Nerve* koopa2dJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01C6BB30);

    //TRex
    al::Nerve* trexHackJump = (al::Nerve*) exl::util::modules::GetTargetOffset(0x01CACDE8);

    if(
        nerve == pahJumpNerve ||
        nerve == pahLongJump ||

        nerve == psnwjWallJump ||
        nerve == pshsdive ||

        //nerve == psljJump ||

        nerve == kshJump ||
        nerve == kshHigh ||
        nerve == kshTurnJump ||
        nerve == kshJumpBurn ||

        nerve == fshJump ||
        nerve == fshHighJump ||
        nerve == fshTrampleJump ||

        nerve == captureJump ||
        nerve == captureCaptureJumpOut ||

        nerve == motorcycleJump ||
        nerve == motorcycleRideRunJump ||
        nerve == motorcycleRideWaitJump ||

        nerve == koopaJumpStart ||

        nerve == koopa2dJump ||

        nerve == trexHackJump
    ){
        return true;
    }

    return false;
};

HOOK_DEFINE_TRAMPOLINE(SetNerveHook){
    static void Callback(al::IUseNerve* user, al::Nerve* nerve){
        if(isJumpingNerve(nerve)) {
            int jumpRemain = JumpData::getJumpRemain();
            al::Nerve* currentNerve = const_cast<al::Nerve*>(user->getNerveKeeper()->getCurrentNerve());

            if(jumpRemain <= 0){
                needPlayJumpSE = true;
                if(isJumpingNerve(currentNerve)){
                    return;
                }else{
                    Orig(user, currentNerve);
                    return;
                }
            }
            JumpData::updateJumpRemain();
        }
        Orig(user, nerve);
    }
};

HOOK_DEFINE_TRAMPOLINE(PlayerActorHakoniwaHack){
    static void Callback(PlayerActorHakoniwa* thisPtr){
        PlayerHackKeeper *playerHackKeeper = thisPtr->getPlayerHackKeeper();

        if(al::isFirstStep(thisPtr)){
            SDLogger::log("Now in hack : %s", playerHackKeeper->getCurrentHackName());

            // Able to swing the controller if the player is in Bowser (Koopa) hack
            if(strcmp(playerHackKeeper->getCurrentHackName(), "Koopa") == 0) {
                playerCanSwing = true;
            }else{
                playerCanSwing = false;
            }
        }
        
        Orig(thisPtr);
    }
};

//-------------
// LAYOUT PART
//-------------
HOOK_DEFINE_TRAMPOLINE(ConstructStageSceneLayout){
    static void Callback(StageSceneLayout* thisPtr, const char* char1, const al::LayoutInitInfo& layoutInitInfo, const al::PlayerHolder* playerHolder, const al::SubCameraRenderer* subCameraRenderer){
        Orig(thisPtr, char1, layoutInitInfo, playerHolder, subCameraRenderer);
        jumpCounter = new JumpCounter("JumpCounter", layoutInitInfo);
        JumpData::instance().setJumpCounter(jumpCounter);
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

/*HOOK_DEFINE_TRAMPOLINE(PakupakuCheckJumpCondition){
    static void Callback(void* thisPtr){
        if(JumpData::getJumpRemain() > 0) Orig(thisPtr);        
    }
};*/

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();

    GameSystemInit::InstallAtOffset(0x535850);

    StageSceneControl::InstallAtSymbol("_ZN10StageScene7controlEv");

    // Jump capture
    PlayerActorHakoniwaControl::InstallAtSymbol("_ZN19PlayerActorHakoniwa7controlEv");
    PlayerInputFunctionIsTriggerJump::InstallAtSymbol("_ZN19PlayerInputFunction13isTriggerJumpEPKN2al9LiveActorEi");
    JoyPadAccelPoseAnalyzerIsSwingAnyHand::InstallAtSymbol("_ZNK2al23JoyPadAccelPoseAnalyzer14isSwingAnyHandEv");
    RSIsCollisionCodeJump::InstallAtSymbol("_ZN2rs19isCollisionCodeJumpEPK19IUsePlayerCollision");

    SetNerveHook::InstallAtSymbol("_ZN2al8setNerveEPNS_9IUseNerveEPKNS_5NerveE");

    ConstructStageSceneLayout::InstallAtSymbol("_ZN16StageSceneLayoutC1EPKcRKN2al14LayoutInitInfoEPKNS2_12PlayerHolderEPKNS2_17SubCameraRendererE");
    CoinCounterTryStart::InstallAtSymbol("_ZN11CoinCounter8tryStartEv");
    CoinCounterTryEnd::InstallAtSymbol("_ZN11CoinCounter6tryEndEv");

    KuriboWingJump::initHooks();
    KaronWingJump::initHooks();
    PukupukuJump::initHooks();

    PlayerActorHakoniwaHack::InstallAtSymbol("_ZN19PlayerActorHakoniwa7exeHackEv");

    SDLogger::instance().init();

    JumpData::instance();
    JumpData::readFromSave();

    SaveJump::initHooks();
};