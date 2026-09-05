#pragma once

#include "Scene.h"
#include "AudioSystem.h"
#include "GameplayFeedbackFX.h"

class CoinRushGameMode
{
public:
    bool active =
        false;

    bool won =
        false;

    bool lost =
        false;

    int totalCount =
        0;

    int collectedCount =
        0;

    float collectRadius =
        1.3f;

    float timeLimit =
        7.0f * 60.0f;

    float timeRemaining =
        7.0f * 60.0f;

public:
    void Reset(
        Scene& scene
    );

    void Recalculate(
        Scene& scene
    );

    void Update(
        Scene& scene,
        AudioSystem& audioSystem,
        GameplayFeedbackFX& feedbackFX,
        float deltaTime
    );

    bool IsFinished() const;

private:
    bool winSoundPlayed =
        false;

    bool loseSoundPlayed =
        false;
};