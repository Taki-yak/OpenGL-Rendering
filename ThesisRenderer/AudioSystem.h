#pragma once

#include <string>
#include <unordered_map>

#include "ThirdParty/miniaudio/miniaudio.h"

class AudioSystem
{
public:
    bool Initialize();

    bool LoadSound(
        const std::string& soundName,
        const std::string& filePath,
        bool loop
    );

    void Play(
        const std::string& soundName,
        float volume = 1.0f
    );

    void PlayFromStart(
        const std::string& soundName,
        float volume = 1.0f
    );

    void Stop(
        const std::string& soundName
    );

    void Shutdown();

private:
    ma_engine engine;
    bool initialized = false;

    std::unordered_map<
        std::string,
        ma_sound*
    > sounds;
};