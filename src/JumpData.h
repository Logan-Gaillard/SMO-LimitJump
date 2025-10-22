#pragma once

#include <basis/seadTypes.h>
#include "Layout/JumpCounter.h"

class JumpData {
    public:
        JumpData();
        static JumpData& instance();

        static const int getJumpRemain(){ return instance().mJumpRemain;}
        static int readFromSave();
        static void updateJumpRemain(bool decrease = true, int newRemain = 1);

        void setJumpCounter(JumpCounter* jumpCounter){jumpCounterLayout = jumpCounter;}
        JumpCounter* getJumpCounter(){ return jumpCounterLayout;}

    private:
        static void setJumpRemain(int remain){ instance().mJumpRemain = remain;}
        static void increaseJumpRemain(int amount){ instance().mJumpRemain += amount;}
        static void decreaseJumpRemain(int amount){ instance().mJumpRemain -= amount;}
        JumpCounter *jumpCounterLayout;
        int mJumpRemain =-1;
};
