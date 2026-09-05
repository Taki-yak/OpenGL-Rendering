#include "MonsterEscapeGameMode.h"

#include "SceneObject.h"

#include "imgui.h"

#include <glm/glm.hpp>

#include <cmath>
#include <iostream>
#include <string>

namespace
{
    bool IsTriggerZoneObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "TriggerZone")
            return true;

        if (object->name.find("Trigger Zone") != std::string::npos)
            return true;

        return false;
    }

    bool IsMonsterSpawnObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "MonsterSpawn")
            return true;

        if (object->name.find("Monster Spawn") != std::string::npos)
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

    SceneObject* FindMonsterSpawnObjectLocal(
        Scene& scene
    )
    {
        for (SceneObject* object : scene.objects)
        {
            if (object == nullptr)
                continue;

            if (IsMonsterSpawnObjectLocal(object))
                return object;
        }

        return nullptr;
    }
}

void MonsterEscapeGameMode::ResetForPlay(
    Scene& scene
)
{
    active =
        false;

    playerCaught =
        false;

    eventSoundPlayed =
        false;

    chaseMusicStarted =
        false;

    eventText =
        "Monster Event: Find the trigger zone.";

    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (IsMonsterSpawnObjectLocal(object))
        {
            object->visible =
                false;
        }

        if (IsTriggerZoneObjectLocal(object))
        {
            object->visible =
                true;
        }
    }
}

void MonsterEscapeGameMode::RestoreForEditor(
    Scene& scene
)
{
    active =
        false;

    playerCaught =
        false;

    eventSoundPlayed =
        false;

    chaseMusicStarted =
        false;

    eventText =
        "Monster Event: Waiting for trigger.";

    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (IsMonsterSpawnObjectLocal(object))
        {
            object->visible =
                true;
        }

        if (IsTriggerZoneObjectLocal(object))
        {
            object->visible =
                true;
        }
    }
}

void MonsterEscapeGameMode::Activate(
    Scene& scene,
    AudioSystem& audioSystem,
    GameplayFeedbackFX& feedbackFX
)
{
    active =
        true;

    playerCaught =
        false;

    eventText =
        "Monster Event: Monster awakened!";

    feedbackFX.ShowNotification(
        "Monster Event Started!",
        ImVec4(
            1.0f,
            0.15f,
            0.15f,
            1.0f
        ),
        1.80f
    );

    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (IsMonsterSpawnObjectLocal(object))
        {
            object->visible =
                true;
        }
    }

    if (!eventSoundPlayed)
    {
        audioSystem.PlayFromStart(
            "monster_zone",
            1.0f
        );

        eventSoundPlayed =
            true;
    }

    std::cout
        << "Monster event activated."
        << std::endl;
}

void MonsterEscapeGameMode::UpdateTrigger(
    Scene& scene,
    AudioSystem& audioSystem,
    GameplayFeedbackFX& feedbackFX
)
{
    if (active)
        return;

    SceneObject* player =
        FindPlayerObjectLocal(
            scene
        );

    if (player == nullptr)
        return;

    for (SceneObject* object : scene.objects)
    {
        if (!IsTriggerZoneObjectLocal(object))
            continue;

        glm::vec3 playerPosition =
            player->transform.position;

        glm::vec3 triggerPosition =
            object->transform.position;

        float distance =
            glm::distance(
                glm::vec3(
                    playerPosition.x,
                    0.0f,
                    playerPosition.z
                ),
                glm::vec3(
                    triggerPosition.x,
                    0.0f,
                    triggerPosition.z
                )
            );

        float finalTriggerRadius =
            glm::max(
                triggerRadius,
                glm::max(
                    object->transform.scale.x,
                    object->transform.scale.z
                ) *
                0.65f
            );

        if (distance <= finalTriggerRadius)
        {
            Activate(
                scene,
                audioSystem,
                feedbackFX
            );

            return;
        }
    }
}

void MonsterEscapeGameMode::UpdateChase(
    Scene& scene,
    AudioSystem& audioSystem,
    GameplayFeedbackFX& feedbackFX,
    float deltaTime,
    TerrainHeightFunction getTerrainHeight
)
{
    if (!active)
        return;

    if (playerCaught)
        return;

    SceneObject* player =
        FindPlayerObjectLocal(
            scene
        );

    SceneObject* monster =
        FindMonsterSpawnObjectLocal(
            scene
        );

    if (
        player == nullptr ||
        monster == nullptr
        )
    {
        return;
    }

    monster->visible =
        true;

    if (!chaseMusicStarted)
    {
        audioSystem.PlayFromStart(
            "monster_chase",
            0.85f
        );

        chaseMusicStarted =
            true;
    }

    glm::vec3 monsterPosition =
        monster->transform.position;

    glm::vec3 playerPosition =
        player->transform.position;

    glm::vec3 direction =
        glm::vec3(
            playerPosition.x - monsterPosition.x,
            0.0f,
            playerPosition.z - monsterPosition.z
        );

    float distance =
        glm::length(
            direction
        );

    if (distance <= catchRadius)
    {
        playerCaught =
            true;

        eventText =
            "Monster Event: LOSE - Monster caught the player!";

        audioSystem.Stop(
            "monster_chase"
        );

        audioSystem.Stop(
            "music_rescue"
        );

        audioSystem.PlayFromStart(
            "coin_lose",
            1.0f
        );

        feedbackFX.ShowNotification(
            "Monster caught you!",
            ImVec4(
                1.0f,
                0.12f,
                0.12f,
                1.0f
            ),
            2.2f
        );

        std::cout
            << "Player lost. Monster caught the player."
            << std::endl;

        return;
    }

    if (distance < 0.001f)
        return;

    direction =
        glm::normalize(
            direction
        );

    monster->transform.position +=
        direction *
        chaseSpeed *
        deltaTime;

    if (getTerrainHeight != nullptr)
    {
        monster->transform.position.y =
            getTerrainHeight(
                monster->transform.position.x,
                monster->transform.position.z
            ) +
            terrainOffset;
    }

    monster->transform.rotation.y =
        glm::degrees(
            std::atan2(
                direction.x,
                direction.z
            )
        );

    eventText =
        "Monster Event: Monster is chasing the player!";
}

bool MonsterEscapeGameMode::IsFinished() const
{
    return
        playerCaught;
}