#pragma once
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/LiveActor/LiveActor.h"

class CoinJump : public al::LiveActor {
    public:
        CoinJump(const char* name): LiveActor(name){}

        void init(const al::ActorInitInfo& info) override;
        void initAfterPlacement() override;
        void control() override;


        void exeWait();
        void exeGot();
        void exeEnd();

        bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) override;

        static al::LiveActor* createActor(const char* name){
            return new CoinJump(name);
        }

    private:
        al::MtxConnector* mMtxConnector = nullptr;
};