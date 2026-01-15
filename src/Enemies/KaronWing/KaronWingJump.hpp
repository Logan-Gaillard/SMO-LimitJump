#pragma once

class KaronWingJump {
public:
    KaronWingJump() = default;

    static const char* name;
    static const int decreaseDuration;

    static KaronWingJump &instance(){
        static KaronWingJump instance;
        return instance;
    }

    static void initHooks();
    void decreaseCooldown(){ cooldown--;}
    void resetCooldown(){ cooldown = decreaseDuration;}
    int cooldown;
};