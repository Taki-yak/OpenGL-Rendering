#pragma once

#include "Scene.h"
#include "AudioSystem.h"
#include "GameplayFeedbackFX.h"

#include <string>

class MonsterEscapeGameMode
{
public:
    using TerrainHeightFunction =
        float (*)(float x, float z);

public:
    bool active =
        false;

    bool playerCaught =
        false;

    float triggerRadius =
        3.8f;

    float chaseSpeed =
        6.5f;

    float catchRadius =
        1.6f;

    float terrainOffset =
        2.0f;

    std::string eventText =
        "Monster Event: Waiting for trigger.";

public:
    void ResetForPlay(
        Scene& scene
    );

    void RestoreForEditor(
        Scene& scene
    );

    void UpdateTrigger(
        Scene& scene,
        AudioSystem& audioSystem,
        GameplayFeedbackFX& feedbackFX
    );

    void UpdateChase(
        Scene& scene,
        AudioSystem& audioSystem,
        GameplayFeedbackFX& feedbackFX,
        float deltaTime,
        TerrainHeightFunction getTerrainHeight
    );

    bool IsFinished() const;

private:
    bool eventSoundPlayed =
        false;

    bool chaseMusicStarted =
        false;

private:
    void Activate(
        Scene& scene,
        AudioSystem& audioSystem,
        GameplayFeedbackFX& feedbackFX
    );
};