#pragma once

#include <basis/seadTypes.h>

class JumpData {
    public:
        JumpData();
        static JumpData& instance();

        static void setJumpRemain(int remain){ instance().mJumpRemain = remain;}
        static void increaseJumpRemain(int amount){ instance().mJumpRemain += amount;}
        static void decreaseJumpRemain(int amount){ instance().mJumpRemain -= amount;}

        static const int getJumpRemain(){ return instance().mJumpRemain;}

        static int readFromSave();

    private:
        int mJumpRemain =-1;
};
