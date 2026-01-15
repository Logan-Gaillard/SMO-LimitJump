#include "PukupukuJump.hpp"
#include "Enemies/Pukupuku/PukupukuJump.hpp"
#include "JumpData.h"
#include "hook/trampoline.hpp"


HOOK_DEFINE_TRAMPOLINE(PukupukuCheckJumpOutCondition){
    static bool Callback(void* thisPtr){
        if(JumpData::getJumpRemain() <= 0) {
            return false;
        }
        return Orig(thisPtr);
    }   
};  

void PukupukuJump::initHooks(){
    PukupukuCheckJumpOutCondition::InstallAtSymbol("_ZNK8Pukupuku21checkJumpOutConditionEv");
}