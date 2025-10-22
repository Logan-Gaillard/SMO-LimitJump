#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al{
    class LiveActor;
}

class IUsePlayerHack;
class HackerStateWingFly;


class KuriboWingHackState : public al::ActorStateBase {
public:
    KuriboWingHackState(const char* name);

    void appear() override;

    void control() override;

    IUsePlayerHack* mPlayerHack;
    HackerStateWingFly* mHackerStateWingFly;
    
};
