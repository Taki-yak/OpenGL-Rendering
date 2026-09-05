#include "CoinRushGameMode.h"

#include "SceneObject.h"

#include <glm/glm.hpp>

#include <iostream>
#include <string>

namespace
{
    bool IsCoinObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "Coin")
            return true;

        if (object->name.find("Coin") != std::string::npos)
            return true;

        return false;
    }

    SceneObject* FindPlayerObjectLocal(
        Scene& scene
    )
    {
        for (SceneObject* object : scene.objects)
        {
            if (object == nullptr)
                continue;

            if (object->name == "Player")
                return object;

            if (object->editorGameplayType == "Player")
                return object;
        }

        return nullptr;
    }
}

void CoinRushGameMode::Reset(
    Scene& scene
)
{
    won =
        false;

    lost =
        false;

    winSoundPlayed =
        false;

    loseSoundPlayed =
        false;

    timeRemaining =
        timeLimit;

    for (SceneObject* object : scene.objects)
    {
        if (IsCoinObjectLocal(object))
        {
            object->visible =
                true;
        }
    }

    Recalculate(
        scene
    );
}

void CoinRushGameMode::Recalculate(
    Scene& scene
)
{
    totalCount =
        0;

    collectedCount =
        0;

    for (SceneObject* object : scene.objects)
    {
        if (!IsCoinObjectLocal(object))
            continue;

        totalCount++;

        if (!object->visible)
        {
            collectedCount++;
        }
    }

    active =
        totalCount > 0;
}

void CoinRushGameMode::Update(
    Scene& scene,
    AudioSystem& audioSystem,
    GameplayFeedbackFX& feedbackFX,
    float deltaTime
)
{
    Recalculate(
        scene
    );

    if (!active)
        return;

    SceneObject* player =
        FindPlayerObjectLocal(
            scene
        );

    if (player == nullptr)
        return;

    bool collectedCoinThisFrame =
        false;

    if (
        !won &&
        !lost
        )
    {
        timeRemaining -=
            deltaTime;

        if (timeRemaining < 0.0f)
        {
            timeRemaining =
                0.0f;
        }
    }

    if (
        !won &&
        !lost
        )
    {
        for (SceneObject* object : scene.objects)
        {
            if (!IsCoinObjectLocal(object))
                continue;

            if (!object->visible)
                continue;

            glm::vec3 playerPosition =
                player->transform.position;

            glm::vec3 coinPosition =
                object->transform.position;

            float distance =
                glm::distance(
                    glm::vec3(
                        playerPosition.x,
                        0.0f,
                        playerPosition.z
                    ),
                    glm::vec3(
                        coinPosition.x,
                        0.0f,
                        coinPosition.z
                    )
                );

            if (distance <= collectRadius)
            {
                glm::vec3 feedbackPosition =
                    object->transform.position +
                    glm::vec3(
                        0.0f,
                        2.2f,
                        0.0f
                    );

                feedbackFX.AddFloatingText(
                    feedbackPosition,
                    "+1 Coin",
                    ImVec4(
                        1.0f,
                        0.85f,
                        0.15f,
                        1.0f
                    )
                );

                feedbackFX.ShowNotification(
                    "Coin Collected!",
                    ImVec4(
                        1.0f,
                        0.85f,
                        0.15f,
                        1.0f
                    ),
                    0.90f
                );

                object->visible =
                    false;

                collectedCount++;

                collectedCoinThisFrame =
                    true;

                std::cout
                    << "Coin collected: "
                    << collectedCount
                    << " / "
                    << totalCount
                    << std::endl;
            }
        }
    }

    if (collectedCoinThisFrame)
    {
        audioSystem.PlayFromStart(
            "coin_collect",
            0.90f
        );
    }

    if (
        totalCount > 0 &&
        collectedCount >= totalCount &&
        !won &&
        !lost
        )
    {
        won =
            true;

        if (!winSoundPlayed)
        {
            audioSystem.PlayFromStart(
                "coin_win",
                0.95f
            );

            winSoundPlayed =
                true;
        }

        feedbackFX.ShowNotification(
            "Coin Rush Complete!",
            ImVec4(
                0.2f,
                1.0f,
                0.2f,
                1.0f
            ),
            2.2f
        );

        std::cout
            << "Coin Rush completed!"
            << std::endl;
    }

    if (
        timeRemaining <= 0.0f &&
        collectedCount < totalCount &&
        !won &&
        !lost
        )
    {
        lost =
            true;

        if (!loseSoundPlayed)
        {
            audioSystem.PlayFromStart(
                "coin_lose",
                0.95f
            );

            loseSoundPlayed =
                true;
        }

        feedbackFX.ShowNotification(
            "Coin Rush Failed!",
            ImVec4(
                1.0f,
                0.15f,
                0.15f,
                1.0f
            ),
            2.2f
        );

        std::cout
            << "Coin Rush failed. Time is over."
            << std::endl;
    }
}

bool CoinRushGameMode::IsFinished() const
{
    return
        won ||
        lost;
}