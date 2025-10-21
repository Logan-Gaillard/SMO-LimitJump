#pragma once

#include <basis/seadTypes.h>

#include "Library/Layout/LayoutActor.h"

namespace al {
    class LayoutInitInfo;
    class LiveActor;
}

class JumpCounter : public al::LayoutActor {
    public:
        JumpCounter(const char* name, const al::LayoutInitInfo& initInfo);

        void kill() override;

        void updatePanel(u32 JumpRemain);
        bool isWait() const;

        void tryStart();
        void updateCountImmidiate();
        void tryEnd();
        void startNoAuthAnim();
        bool tryUpdateCount();

        s32 getCountFromData() const;

        
        //Les nerves
        void exeAppear();
        void exeWait();
        void exeEnd();
        void exeAdd();
        void exeSub();
        void exeNoAuth();

    private:
        s32 mPrevJumpRemain = 0;
        s32 mJumpRemain = 0;
        s32 mAnimationCount = 0;

        bool mIsUpdateCount = false;

        const char* mPanelName = nullptr;
};