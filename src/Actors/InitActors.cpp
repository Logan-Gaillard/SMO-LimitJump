#include "InitActors.hpp"
#include "Actors/Item/CoinJump/CoinJump.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/GameDataHolderBase.h"
#include "Library/Scene/Scene.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"
#include "hook/trampoline.hpp"
#include "logger/SDLogger.hpp"
#include "System/GameDataFunction.h"

namespace InitActors {
    al::LiveActor* mCoinJump = nullptr; 
}

int getAmountOfCoinJump(const al::Scene* scene){
    if(GameDataFunction::isMainStage(scene)){
        if(GameDataFunction::isWorldCap(scene)) return 2;
        if(GameDataFunction::isWorldWaterfall(scene)) return 2;
        if(GameDataFunction::isWorldSand(scene)) return 5;
        if(GameDataFunction::isWorldForest(scene)) return 5;
        if(GameDataFunction::isWorldLake(scene)) return 2;
        if(GameDataFunction::isWorldCloud(scene)) return 1;
        if(GameDataFunction::isWorldClash(scene)) return 3;
        if(GameDataFunction::isWorldCity(scene)) return 4;
        if(GameDataFunction::isWorldSnow(scene)) return 3;
        if(GameDataFunction::isWorldSea(scene)) return 3;
        if(GameDataFunction::isWorldBoss(scene)) return 1;
        if(GameDataFunction::isWorldSky(scene)) return 4;
        if(GameDataFunction::isWorldMoon(scene)) return 2;
        if(GameDataFunction::isWorldPeach(scene)) return 3;
        if(GameDataFunction::isWorldSpecial1(scene)) return 1;
        if(GameDataFunction::isWorldSpecial2(scene)) return 1;
    }
    return 0;
}

void initCoinJumpActors(const al::Scene* scene, al::ActorInitInfo* actorInitInfo){
    int amount = getAmountOfCoinJump(scene);

    SDLogger::log("Création des CoinJump, amount: %i", amount);
    for(int i = 0; i < amount; i++){
        al::LiveActor* coinJump = new CoinJump("CoinJump", false);
        al::initCreateActorNoPlacementInfo(coinJump, *actorInitInfo);
    }
}


HOOK_DEFINE_TRAMPOLINE(initActorInitInfo){
    static void Callback(al::ActorInitInfo* actorInitInfo, const al::Scene* scene,
                       const al::PlacementInfo* placementInfo, const al::LayoutInitInfo* layoutInfo,
                       const al::ActorFactory* actorFactory, al::SceneMsgCtrl* sceneMsgCtrl,
                       al::GameDataHolderBase* gameDataHolderBase){
        SDLogger::log("InitActorInitInfo");

        Orig(actorInitInfo, scene, placementInfo, layoutInfo, actorFactory, sceneMsgCtrl, gameDataHolderBase);

        initCoinJumpActors(scene, actorInitInfo);
    }
};

void InitActors::initHook() {
    initActorInitInfo::InstallAtSymbol("_ZN2al17initActorInitInfoEPNS_13ActorInitInfoEPKNS_5SceneEPKNS_13PlacementInfoEPKNS_14LayoutInitInfoEPKNS_12ActorFactoryEPNS_12SceneMsgCtrlEPNS_18GameDataHolderBaseE");
}