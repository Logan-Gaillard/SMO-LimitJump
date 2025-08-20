#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al{
    class WaterSurfaceFinder;
}

class PlayerConst;
class PlayerAnimator;
class PlayerInput;
class IUsePlayerCollision;
class PlayerTrigger;
class PlayerCarryKeeper;
class PlayerModelHolder;
class PlayerExternalVelocity;
class PlayerSpinCapAttack;
class PlayerEffect;
class PlayerJointParamSwim;


class PlayerStateSwim : public al::ActorStateBase {
public:
    PlayerStateSwim(al::LiveActor* player,
                     const PlayerConst* pConst,
                     const IUsePlayerCollision* playerCollision,
                     const PlayerInput* playerInput,
                     const PlayerTrigger* playerTrigger,
                     const PlayerCarryKeeper* playerCarryKeeper,
                     const PlayerModelHolder* playerModelHolder,
                     const PlayerExternalVelocity* playerExternalVelocity,
                     PlayerAnimator* playerAnimator,
                     PlayerSpinCapAttack* playerSpinCapAttack,
                     const al::WaterSurfaceFinder* waterSurfaceFinder,
                     PlayerEffect* playerEffect,
                     PlayerJointParamSwim* playerJointParamSwim);

    void appear() override;

    bool isSurface() const;

};
