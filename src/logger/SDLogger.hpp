#pragma once
#define SD_LOGGER_PATH "sd:/logs.txt"

#include "types.h"
#include <nn/fs.h>

#include "log/svc_logger.hpp"

enum class SDLoggerState {
    UNINITIALIZED,
    INITIALIZED,
    ERROR
};

class SDLogger {
public:
    SDLogger() = default;

    static SDLogger& instance();

    static void log(const char* message, ...);

    bool init();

    static s64 getSize(){
        s64 size;
        nn::fs::GetFileSize(&size, instance().handle);
        return size;
    }

    SDLoggerState getState(){
        return state;
    };

private:
    static bool openLogFile();

    bool setSize(s64 size);

    SDLoggerState state = SDLoggerState::UNINITIALIZED;

    nn::fs::FileHandle handle;
    nn::fs::WriteOption option;

    exl::log::SvcLogger svcLogger;

    const char* path = SD_LOGGER_PATH;
};