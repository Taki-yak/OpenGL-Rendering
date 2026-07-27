#include "AudioSystem.h"
#include <iostream>

bool AudioSystem::Initialize()
{
    ma_result result =
        ma_engine_init(
            nullptr,
            &engine
        );

    if (result != MA_SUCCESS)
    {
        std::cout
            << "AudioSystem failed to initialize."
            << std::endl;

        initialized =
            false;

        return false;
    }

    initialized =
        true;

    std::cout
        << "AudioSystem initialized."
        << std::endl;

    return true;
}

bool AudioSystem::LoadSound(
    const std::string& soundName,
    const std::string& filePath,
    bool loop
)
{
    if (!initialized)
        return false;

    if (sounds.find(soundName) != sounds.end())
    {
        std::cout
            << "Sound already loaded: "
            << soundName
            << std::endl;

        return true;
    }

    ma_sound* sound =
        new ma_sound();

    ma_result result =
        ma_sound_init_from_file(
            &engine,
            filePath.c_str(),
            0,
            nullptr,
            nullptr,
            sound
        );

    if (result != MA_SUCCESS)
    {
        std::cout
            << "Failed to load sound: "
            << filePath
            << std::endl;

        delete sound;

        return false;
    }

    ma_sound_set_looping(
        sound,
        loop ? MA_TRUE : MA_FALSE
    );

    sounds[soundName] =
        sound;

    std::cout
        << "Loaded sound: "
        << soundName
        << " from "
        << filePath
        << std::endl;

    return true;
}

void AudioSystem::Play(
    const std::string& soundName,
    float volume
)
{
    if (!initialized)
        return;

    auto it =
        sounds.find(
            soundName
        );

    if (it == sounds.end())
        return;

    ma_sound_set_volume(
        it->second,
        volume
    );

    ma_sound_start(
        it->second
    );
}

void AudioSystem::PlayFromStart(
    const std::string& soundName,
    float volume
)
{
    if (!initialized)
        return;

    auto it =
        sounds.find(
            soundName
        );

    if (it == sounds.end())
        return;

    ma_sound_stop(
        it->second
    );

    ma_sound_seek_to_pcm_frame(
        it->second,
        0
    );

    ma_sound_set_volume(
        it->second,
        volume
    );

    ma_sound_start(
        it->second
    );
}

void AudioSystem::Stop(
    const std::string& soundName
)
{
    if (!initialized)
        return;

    auto it =
        sounds.find(
            soundName
        );

    if (it == sounds.end())
        return;

    ma_sound_stop(
        it->second
    );
}

void AudioSystem::Shutdown()
{
    if (!initialized)
        return;

    for (auto& soundPair : sounds)
    {
        if (soundPair.second != nullptr)
        {
            ma_sound_stop(
                soundPair.second
            );

            ma_sound_uninit(
                soundPair.second
            );

            delete soundPair.second;
        }
    }

    sounds.clear();

    ma_engine_uninit(
        &engine
    );

    initialized =
        false;

    std::cout
        << "AudioSystem shutdown."
        << std::endl;
}