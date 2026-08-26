#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AnimatedVertex.h"
#include "BoneInfo.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
class AnimatedModel
{
public:
    AnimatedModel(
        const std::string& path
    );
    ~AnimatedModel();

    void DrawStaticPreview(
        Shader& shader
    );
    void UpdateAnimation(
        float deltaTime
    );

    void Play(
        bool resetTime = true
    );

    void Pause();

    void SetLooping(
        bool value
    );

    void SetAnimationSpeed(
        float speed
    );

    bool IsPlaying() const
    {
        return animationPlaying;
    }

    float GetAnimationTime() const
    {
        return animationTime;
    }

    float GetAnimationDuration() const
    {
        return animationDuration;
    }

    float GetAnimationSpeed() const
    {
        return animationSpeed;
    }
    bool HasPreviewMesh() const
    {
        return
            previewReady &&
            previewVertexCount > 0;
    }
    void Load(
        const std::string& path
    );

    bool IsLoaded() const
    {
        return loaded;
    }

    int GetBoneCount() const
    {
        return boneCounter;
    }

    int GetVertexCount() const
    {
        return static_cast<int>(
            vertices.size()
            );
    }

    int GetMeshCount() const
    {
        return meshCount;
    }

    int GetAnimationCount() const
    {
        return animationCount;
    }

    const std::vector<std::string>& GetAnimationNames() const
    {
        return animationNames;
    }

    const std::string& GetPath() const
    {
        return sourcePath;
    }

private:
    Assimp::Importer importer;

    const aiScene* loadedScene =
        nullptr;

    float animationTime =
        0.0f;

    float animationDuration =
        0.0f;

    float ticksPerSecond =
        25.0f;

    float animationSpeed =
        1.0f;

    bool animationPlaying =
        true;

    bool animationLooping =
        true;

    glm::mat4 globalInverseTransform =
        glm::mat4(
            1.0f
        );

    std::vector<AnimatedVertex> previewTriangleVertices;

    std::vector<glm::mat4> finalBoneMatrices;
    std::string sourcePath;

    bool loaded =
        false;

    int meshCount =
        0;

    int animationCount =
        0;
    unsigned int previewVAO =
        0;

    unsigned int previewVBO =
        0;

    int previewVertexCount =
        0;

    bool previewReady =
        false;

    std::vector<float> previewVertexData;
    std::vector<std::string> animationNames;

    std::vector<AnimatedVertex> vertices;

    std::map<std::string, BoneInfo> boneInfoMap;

    int boneCounter =
        0;

    void ProcessNode(
        aiNode* node,
        const aiScene* scene
    );

    void ProcessMesh(
        aiMesh* mesh,
        const aiScene* scene
    );
    void AddPreviewVertex(
        const AnimatedVertex& vertex
    );

    void BuildStaticPreviewMesh();
    void ExtractBoneWeightForVertices(
        aiMesh* mesh,
        const aiScene* scene,
        unsigned int vertexStartIndex
    );

    void SetVertexBoneData(
        AnimatedVertex& vertex,
        int boneID,
        float weight
    );
    void CalculateBoneTransform(
        aiNode* node,
        const glm::mat4& parentTransform
    );

    const aiNodeAnim* FindNodeAnimationChannel(
        aiAnimation* animation,
        const std::string& nodeName
    );

    glm::vec3 InterpolatePosition(
        float animationTime,
        const aiNodeAnim* channel
    );

    glm::quat InterpolateRotation(
        float animationTime,
        const aiNodeAnim* channel
    );

    glm::vec3 InterpolateScale(
        float animationTime,
        const aiNodeAnim* channel
    );

    int GetPositionIndex(
        float animationTime,
        const aiNodeAnim* channel
    );

    int GetRotationIndex(
        float animationTime,
        const aiNodeAnim* channel
    );

    int GetScaleIndex(
        float animationTime,
        const aiNodeAnim* channel
    );

    void UpdateSkinnedPreviewMesh();
};