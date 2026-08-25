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
};