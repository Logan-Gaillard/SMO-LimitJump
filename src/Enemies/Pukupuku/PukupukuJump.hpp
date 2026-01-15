#pragma once

class PukupukuJump {
public:
    PukupukuJump() = default;

    static const char* name;
    static const int decreaseDuration;

    static PukupukuJump &instance(){
        static PukupukuJump instance;
        return instance;
    }

    static void initHooks();
};