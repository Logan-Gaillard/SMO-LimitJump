#include "Actors/Item/CoinJump/CoinJump.h"

#include "JumpData.h"
#include "Layout/JumpCounter.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/CollisionPartsTriangle.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Controller/InputFunction.h"
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
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Se/SeFunction.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "ModeBalloon/TimeBalloonHintArrow.h"
#include "System/GameDataFunction.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"
#include "Library/Player/PlayerUtil.h"

#include "logger/SDLogger.hpp"
#include "math/seadMathCalcCommon.h"
#include "math/seadVectorFwd.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/PartsInterpolator.h"

namespace {
    NERVE_IMPL(CoinJump, Wait)
    NERVE_IMPL(CoinJump, End)
    NERVE_IMPL(CoinJump, Got)
    NERVE_IMPL(CoinJump, FindPlace)
    NERVES_MAKE_STRUCT(CoinJump, Got, End, Wait, FindPlace);
}


bool CoinJump::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self){
    
    if(rs::isMsgItemGetAll(message) && al::isNerve(this, &NrvCoinJump.Wait)){
        al::setNerve(this, &NrvCoinJump.Got);
    }
    return false;
}

void CoinJump::init(const al::ActorInitInfo& info){
    SDLogger::log("CoinJump init");
    al::initActorWithArchiveName(this, info, "CoinJump", 0);
    

    if(!coinPlaced) {
        PosFound = false;

        SDLogger::log("CoinJump init without placement, go to FindPlace nerve");
        al::initNerve(this, &NrvCoinJump.FindPlace, 0);
    } else {
        SDLogger::log("CoinJump init with placement, go to Wait nerve");
        al::initNerve(this, &NrvCoinJump.Wait, 0);
    }

    makeActorAlive();
    al::invalidateClipping(this); // Désactive le clipping pour que la pièce reste active, même si une longue distance du joueur
}

void CoinJump::initAfterPlacement(){
    SDLogger::log("CoinJump initAfterPlacement");

    mPosTrans = al::getTrans(this);
    mPosInit = true;
}

void CoinJump::control(){
    al::rotateQuatYDirDegree(this, 3.0f);
    if(al::isNerve(this, &NrvCoinJump.Wait)){
        //J'utilise pour le mouvement haut/bas : f(x) = a sin(k x)+a

        f32 sizeFactor = 30.0f;

        if(mFlyingPhase >= 10.0f * sead::Mathf::pi()){
            mFlyingPhase = 0.0f; // Reset de la phase pour une boucle infinie
        }

        mFlyingPhase += 0.1f;
        sead::Vector3f translation = mPosTrans;

        f32 offsetY = sizeFactor * sead::Mathf::sin(0.2f * mFlyingPhase) + sizeFactor;
        
        translation.y += offsetY;
        al::setTrans(this, translation);

    }else if(al::isNerve(this, &NrvCoinJump.Got)){
        mFlyingPhase += 1.0f;
        al::rotateQuatYDirDegree(this, 20.0f); // Rotation plus rapide si capturé
        f32 offsetY = mFlyingPhase * 10.0f;
        sead::Vector3f translation = mPosTrans;
        translation.y += offsetY;
        al::setTrans(this, translation);
    }
}

void CoinJump::exeWait(){
    if(al::isFirstStep(this)){
        SDLogger::log("CoinJump exeWait");
        // Start wait animation
        al::startAction(this, "Wait");
    }
}

void CoinJump::exeEnd(){
    if(al::isFirstStep(this)){
        SDLogger::log("CoinJump exeEnd");
    }
    PosFound = false;
    al::hideModel(this); 
    al::setNerve(this, &NrvCoinJump.FindPlace);
}


void CoinJump::exeGot(){
    if(al::isFirstStep(this)){
        SDLogger::log("CoinJump exeGot");
        JumpData::updateJumpRemain(false, 5); // Ajout 5 sauts supplémentaire
        mFlyingPhase = 0.0f; // Reset de la phase
    }

    if(mFlyingPhase >= 30.0f){
        al::setNerve(this, &NrvCoinJump.End);
    }
}

void CoinJump::exeFindPlace(){
    if(al::isFirstStep(this)){
        SDLogger::log("CoinJump exeFindPlace");
        al::initRandomSeedByTick();
    }

    if(PosFound) return;

    al::LiveActor* player = al::getPlayerActor(this, 0);
    sead::Vector3f playerPos = rs::getPlayerPos(this); // Position du joueur
    sead::Vector3f spawn; // Création de la variable du vecteur finale pour la pièce
    f32 minDist = 500.0f; // Distance minimum de spawn 
    f32 maxDist = 100000.0f; // Distance maximum de spawn
    f32 angle  = al::getRandom(628) / 100.0f; // Angle aléatoire pour la direction du spawn autour du joueur 360°
    f32 radius = minDist + (maxDist - minDist) * (al::getRandom(100) / 100.0f); // Distance aléatoire entre minDist et maxDist

    sead::Vector3f newPos = playerPos; // Position de départ pour le calcul du spawn
    newPos.x += radius * sead::Mathf::cos(angle); // On ajoute pour x, la distance en fonction de l'angle et du rayon
    newPos.z += radius * sead::Mathf::sin(angle); // On ajoute pour z, la distance en fonction de l'angle et du rayon

    const al::ArrowHitInfo* hitInfo = nullptr; // Déclaration de ArrowHitInfo
    // Que fait ArrowHitInfo ?
    // De ce que j'ai compris, il a permettre avec deux points, tracer une ligne et check lister les collisions sur cette ligne

    sead::Vector3f start = newPos; // Premier point de départ pour le raycast, largement grande
    start.y += 5000.0f;
    sead::Vector3f vec(0.0f, -500000.0f, 0.0f); // Deuxième point, vecteur de direction (0, -500000, 0) pour faire une ligne verticale

    al::TriangleFilterGroundOnly filter(al::getGravity(player));

    if (!alCollisionUtil::getFirstPolyOnArrow(this, &hitInfo, start, vec, nullptr, &filter)) return; // Si aucune collision n'est détecté, on annule le spawn

    sead::Vector3f groundPos = alCollisionUtil::getCollisionHitPos(**hitInfo); // Récupération de la position de la collision, donc du sol
    // Comment on sait que c'est le sol et pas un mur ? 
    // En fait, on ne le sait pas, mais comme la ligne est verticale, il y a de grande chance que ce soit le sol.

    groundPos.y += 100.0f;

    spawn = groundPos; // On définit la position de final à la pièce

    al::resetPosition(this, spawn); // On téléporte la pièce à la position de spawn

    mPosTrans = al::getTrans(this);
    mPosInit = true;
    mFlyingPhase = 0.0f;

    al::setNerve(this, &NrvCoinJump.Wait);
    
    al::showModel(this);
    makeActorAlive();

    PosFound = true;
}