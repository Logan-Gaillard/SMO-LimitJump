#pragma once

#include "Library/LiveActor/LiveActor.h"

class CapTargetInfo;
class EnemyStateReset;
class EnemyStateSwoon;
class EnemyStateBlownDown;
class NerveStateBase;
class PlayerHackStartShaderCtrl;
class IUsePlayerHack;

struct KuriboWing : public al::LiveActor {
public:
    KuriboWing(const char* name);
public:
    //void* filler[0x138];  
    CapTargetInfo* mCapTargetInfo;
    EnemyStateReset* mEnemyStateReset1;
    EnemyStateReset* mEnemyStateReset2;
    EnemyStateSwoon* mEnemyStateSwoon;
    EnemyStateBlownDown* mEnemyStateBlownDown;
    NerveStateBase* mNerveStateBase;
    PlayerHackStartShaderCtrl* mPlayerHackStartShaderCtrl;
    IUsePlayerHack* mPlayerHack;
};