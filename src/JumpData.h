#pragma once

#include <basis/seadTypes.h>

class JumpData {
    public:
        JumpData();
        static JumpData& instance();

        static void setJumpRemain(s32 remain){ instance().mJumpRemain = remain;}
        static void increaseJumpRemain(s32 amount){ instance().mJumpRemain += amount;}
        static void decreaseJumpRemain(s32 amount){ instance().mJumpRemain -= amount;}

        static const s32 getJumpRemain(){ return instance().mJumpRemain;}

        static s32 readFromSave();

    private:
        s32 mJumpRemain =-1;
};
