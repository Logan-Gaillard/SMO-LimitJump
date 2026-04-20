#pragma once
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "ModeBalloon/TimeBalloonHintArrow.h"

class CoinJump : public al::LiveActor {
    public:
        CoinJump(const char* name): LiveActor(name){}
        CoinJump(const char* name, bool coinPlaced): LiveActor(name), coinPlaced(coinPlaced){}

        void init(const al::ActorInitInfo& info) override;
        void initAfterPlacement() override;
        void control() override;


        void exeWait();
        void exeGot();
        void exeEnd();
        void exeFindPlace();

        bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) override;

        static al::LiveActor* createActor(const char* name){
            return new CoinJump(name);
        }

    private:
        sead::Vector3f mPosTrans = sead::Vector3f::zero;
        f32 mFlyingPhase = 0.0f;
        bool mPosInit = false;
        bool PosFound = false;
        bool coinPlaced = true;
};