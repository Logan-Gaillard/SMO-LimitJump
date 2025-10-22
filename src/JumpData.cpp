#include "JumpData.h"
#include "logger/SDLogger.hpp"

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


int JumpData::readFromSave(){
    //TODO : Save and read jumps remains
    return 10;
}

void JumpData::updateJumpRemain(bool decrease, int amount){
    if (decrease) {
        if(JumpData::getJumpRemain() > 0)
            JumpData::decreaseJumpRemain(amount);
    } else {
        JumpData::increaseJumpRemain(amount);
    }
    instance().getJumpCounter()->tryUpdateCount();
    SDLogger::log("Il reste %i sauts", getJumpRemain());
}