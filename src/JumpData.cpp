#include "JumpData.h"
#include "logger/SDLogger.hpp"

JumpData::JumpData() {
    mJumpRemain = readFromSave();
}

JumpData& JumpData::instance() { 
    static JumpData instance;
    return instance;
}


int JumpData::readFromSave(){
    return 0;
}

void JumpData::updateJumpRemain(bool decrease, int amount){
    if (decrease) {
        if(JumpData::getJumpRemain() > 0)
            JumpData::decreaseJumpRemain(amount);
    } else {
        JumpData::increaseJumpRemain(amount);
    }

    if(instance().getJumpCounter() != nullptr){
        instance().getJumpCounter()->tryUpdateCount();
    }
    
    SDLogger::log("Il reste %i sauts", getJumpRemain());
}

void JumpData::setJumpRemainFromSave(int remain){
    setJumpRemain(remain);
    if(instance().getJumpCounter() != nullptr){
        instance().getJumpCounter()->tryUpdateCount();
    }
    SDLogger::log("Jump remain set from save: %i", getJumpRemain());
}