#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al{
    class ActorStateBase;
    class LiveActor;
}

class HackerStateWingFly;
class IUsePlayerHack;
class HackerJudgeNormalFall;


class KuriboWingHackState : public al::ActorStateBase {
public:
    KuriboWingHackState(al::LiveActor*, IUsePlayerHack**);
public:
    IUsePlayerHack** mHacker = nullptr;
    HackerStateWingFly* mHackerStateWingFly;
    HackerJudgeNormalFall* mHackerJudgeNormalFall;
};