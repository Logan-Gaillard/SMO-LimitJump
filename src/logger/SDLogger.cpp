#include "logger/SDLogger.hpp"

#include <nn/fs.h>
#include <exlaunch.hpp>
#include "nn/util.h"

//Cette classe doit être un singleton (une seule instance de la classe)
SDLogger& SDLogger::instance() { 
    static SDLogger instance;
    return instance;
}

//Constructeur de la classe
bool SDLogger::init() {
    nn::fs::MountSdCard("sd"); //Monte la carte SD, si je ne le fais pas, ça bug... Je suppose que le jeu ne monte pas la carte SD au lancement
    instance().state = SDLoggerState::INITIALIZED;
    return true;
}

// void SDLogger::log(const char* message) {
//     if(instance().state == SDLoggerState::ERROR || instance().state == SDLoggerState::UNINITIALIZED){
//         instance().svcLogger.LogRaw("Envoie du log via SD impossible, son status ne le permet pas\n");
//         return;
//     }

//     instance().openLogFile();

//     s64 size = getSize();
//     if(instance().setSize(size + std::strlen(message))){
//         instance().svcLogger.LogRaw("Erreur lors de la modification de la taille du fichier\n");
//         instance().state = SDLoggerState::ERROR;
//         return;
//     }

//     if(nn::fs::WriteFile(instance().handle, size, message, std::strlen(message), instance().option).IsFailure()){
//         instance().svcLogger.LogRaw("Erreur lors de l'écriture du log\n");
//         instance().state = SDLoggerState::ERROR;
//         return;
//     }

//     if(nn::fs::FlushFile(instance().handle).IsFailure()){
//         instance().svcLogger.LogRaw("Erreur lors du flush du log\n");
//         instance().state = SDLoggerState::ERROR;
//         return;
//     }

//     nn::fs::CloseFile(instance().handle);

//     instance().svcLogger.LogRaw(message);
// }

void SDLogger::log(const char *message, ...){
    if(instance().state == SDLoggerState::ERROR || instance().state == SDLoggerState::UNINITIALIZED){
        instance().svcLogger.LogRaw("Envoie du log via SD impossible, son status ne le permet pas\n");
        return;
    }

    va_list args;
    va_start(args, message);
    char buffer[0x500];

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), message, args) > 0){
        instance().openLogFile();

        s64 size = getSize();
        if(instance().setSize(size + std::strlen(buffer))){
            instance().svcLogger.LogRaw("Erreur lors de la modification de la taille du fichier\n");
            instance().state = SDLoggerState::ERROR;
            return;
        }

        if(nn::fs::WriteFile(instance().handle, size, buffer, std::strlen(buffer), instance().option).IsFailure()){
            instance().svcLogger.LogRaw("Erreur lors de l'écriture du log\n");
            instance().state = SDLoggerState::ERROR;
            return;
        }

        if(nn::fs::FlushFile(instance().handle).IsFailure()){
            instance().svcLogger.LogRaw("Erreur lors du flush du log\n");
            instance().state = SDLoggerState::ERROR;
            return;
        }

        nn::fs::CloseFile(instance().handle);

        instance().svcLogger.LogRaw(buffer);
    } else {
        instance().svcLogger.LogRaw("Erreur lors de la création du message\n");
        instance().state = SDLoggerState::ERROR;
        return;
    }
}

bool SDLogger::setSize(s64 size) {
    return nn::fs::SetFileSize(handle, size).IsFailure();
}

bool SDLogger::openLogFile() {
    if(nn::fs::OpenFile(&instance().handle, instance().path, nn::fs::OpenMode_ReadWrite).IsFailure()){
        nn::fs::CreateFile("sd:/logs.txt", 0);

        if(nn::fs::OpenFile(&instance().handle, instance().path, nn::fs::OpenMode_ReadWrite).IsFailure()) {
            instance().state = SDLoggerState::ERROR;
            return false;
        } else {instance().svcLogger.LogRaw("Création du fichier log réussi\n");}
    }
    return true;
}