#pragma once

class KuriboWingJump {
public:
    KuriboWingJump() = default;

    static const char* name;
    static const int decreaseDuration;

    static KuriboWingJump &instance(){
        static KuriboWingJump instance;
        return instance;
    }

    void initHooks();
    void decreaseCooldown(){ cooldown--;}
    void resetCooldown(){ cooldown = decreaseDuration;}
    int cooldown;
};