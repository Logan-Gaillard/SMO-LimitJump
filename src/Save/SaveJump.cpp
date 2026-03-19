#include "SaveJump.h"
#include "JumpData.h"
#include "Library/Yaml/Writer/ByamlWriter.h"
#include "Library/Yaml/ByamlUtil.h"
#include "System/GameConfigData.h"
#include "hook/trampoline.hpp"
#include "logger/SDLogger.hpp"

HOOK_DEFINE_TRAMPOLINE(WriteSaveJumpRemaining){
    static void Callback(GameConfigData* thisPtr, al::ByamlWriter* byamlWriter){
        Orig(thisPtr, byamlWriter);
        SDLogger::log("Writing jump remain to save: %i", JumpData::getJumpRemain());
        int jumpRemain = JumpData::getJumpRemain();
        byamlWriter->addInt("JumpRemain", jumpRemain);
    }
};

HOOK_DEFINE_TRAMPOLINE(ReadSaveJumpRemaining){
    static void Callback(GameConfigData* thisPtr, const al::ByamlIter& save){
        Orig(thisPtr, save);
        SDLogger::log("Reading jump remain from save");
        int jumpRemain = 0;
        if(al::tryGetByamlS32(&jumpRemain, save, "JumpRemain")){
            JumpData::setJumpRemainFromSave(jumpRemain);
        }else{
            JumpData::setJumpRemainFromSave(0);
        }
    }
};

void SaveJump::initHooks(){
    WriteSaveJumpRemaining::InstallAtSymbol("_ZN14GameConfigData5writeEPN2al11ByamlWriterE");
    ReadSaveJumpRemaining::InstallAtSymbol("_ZN14GameConfigData4readERKN2al9ByamlIterE");
}