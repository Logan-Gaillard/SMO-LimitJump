#include "Item/CoinJump/CoinJump.h"

#include "JumpData.h"
#include "Layout/JumpCounter.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/CollisionPartsTriangle.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Demo/DemoFunction.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Se/SeFunction.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "System/GameDataFunction.h"
#include "Util/SensorMsgFunction.h"

#include "logger/SDLogger.hpp"

namespace {
    NERVE_IMPL(CoinJump, Wait)
    NERVE_IMPL(CoinJump, End)
    NERVE_IMPL(CoinJump, Got)
    NERVES_MAKE_STRUCT(CoinJump, Got, End, Wait);
}

bool CoinJump::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self){
    //if(!al::isSensorPlayer(other)) return false;
    
    if(rs::isMsgItemGetAll(message) && al::isNerve(this, &NrvCoinJump.Wait)){
        al::setNerve(this, &NrvCoinJump.Got);
    }
    return false;
}

void CoinJump::init(const al::ActorInitInfo& info){
    SDLogger::log("JumpCoin init");
    al::initActorWithArchiveName(this, info, "Coin", 0);
    al::initNerve(this, &NrvCoinJump.Wait, 0);
    al::offCollide(this);
    
    mMtxConnector = al::createMtxConnector(this);



    makeActorAlive();
}

void CoinJump::initAfterPlacement(){
    if(mMtxConnector){
        sead::Vector3f frontDir = sead::Vector3f(0.0f, 0.0f, 0.0f);
        al::calcFrontDir(&frontDir, this);
        al::attachMtxConnectorToCollision(mMtxConnector, this, al::getTrans(this) + frontDir * 50.0f, frontDir * -400.0f);
    }
}

void CoinJump::control(){
    
    al::rotateQuatYDirRandomDegree(this);
}

void CoinJump::exeWait(){
    if(al::isFirstStep(this)){
        // Start wait animation
        al::startAction(this, "Wait");
        SDLogger::log("JumpCoin exeWait");
    }
}

void CoinJump::exeEnd(){
    kill();
}


void CoinJump::exeGot(){
    if(al::isFirstStep(this)){
        SDLogger::log("JumpCoin exeGot");
        JumpData::updateJumpRemain(false, 5); //Increase jump remain by 1
    }

    al::setNerve(this, &NrvCoinJump.End);
}