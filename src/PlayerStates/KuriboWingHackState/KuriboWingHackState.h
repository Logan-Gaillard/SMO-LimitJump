#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al{
    class LiveActor;
}

class IUsePlayerHack;
class HackerStateWingFly;


class KuriboWingHackState : public al::ActorStateBase {
public:
    KuriboWingHackState(al::LiveActor* liveActor, IUsePlayerHack* playerHack);

    void appear() override;

    void control() override;

    IUsePlayerHack* mPlayerHack;
    HackerStateWingFly* mHackerStateWingFly;
    
};
