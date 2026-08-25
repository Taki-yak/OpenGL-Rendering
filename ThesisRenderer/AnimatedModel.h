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

class AnimatedModel
{
public:
    AnimatedModel(
        const std::string& path
    );

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
    std::string sourcePath;

    bool loaded =
        false;

    int meshCount =
        0;

    int animationCount =
        0;

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
};