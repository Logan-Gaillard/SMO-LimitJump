#include "JumpData.h"

JumpData::JumpData() {
    mJumpRemain = readFromSave();
    if(mJumpRemain < 0) {
        mJumpRemain = 0;
    }
}

JumpData& JumpData::instance() { 
    static JumpData instance;
    return instance;
}


s32 JumpData::readFromSave(){
    return (s32)10;
}