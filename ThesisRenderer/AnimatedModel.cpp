#include "AnimatedModel.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
AnimatedModel::AnimatedModel(const std::string& path)
{
    Load(path);
}
AnimatedModel::~AnimatedModel()
{
    if (previewVBO != 0)
    {
        glDeleteBuffers(
            1,
            &previewVBO
        );

        previewVBO =
            0;
    }

    if (previewVAO != 0)
    {
        glDeleteVertexArrays(
            1,
            &previewVAO
        );

        previewVAO =
            0;
    }
}

void AnimatedModel::AddPreviewVertex(
    const AnimatedVertex& vertex
)
{
    previewVertexData.push_back(
        vertex.position.x
    );

    previewVertexData.push_back(
        vertex.position.y
    );

    previewVertexData.push_back(
        vertex.position.z
    );

    previewVertexData.push_back(
        vertex.normal.x
    );

    previewVertexData.push_back(
        vertex.normal.y
    );

    previewVertexData.push_back(
        vertex.normal.z
    );

    previewVertexData.push_back(
        vertex.texCoords.x
    );

    previewVertexData.push_back(
        vertex.texCoords.y
    );
}

void AnimatedModel::BuildStaticPreviewMesh()
{
    previewReady =
        false;

    previewVertexCount =
        0;

    if (previewVertexData.empty())
    {
        std::cout
            << "Animated preview mesh has no vertex data."
            << std::endl;

        return;
    }

    if (previewVBO != 0)
    {
        glDeleteBuffers(
            1,
            &previewVBO
        );

        previewVBO =
            0;
    }

    if (previewVAO != 0)
    {
        glDeleteVertexArrays(
            1,
            &previewVAO
        );

        previewVAO =
            0;
    }

    glGenVertexArrays(
        1,
        &previewVAO
    );

    glGenBuffers(
        1,
        &previewVBO
    );

    glBindVertexArray(
        previewVAO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        previewVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        previewVertexData.size() *
        sizeof(float),
        previewVertexData.data(),
        GL_STATIC_DRAW
    );

    int stride =
        8 *
        sizeof(float);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)0
    );

    glEnableVertexAttribArray(
        0
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(
        1
    );

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(6 * sizeof(float))
    );

    glEnableVertexAttribArray(
        2
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(
        0
    );

    previewVertexCount =
        static_cast<int>(
            previewVertexData.size() / 8
            );

    previewReady =
        true;

    std::cout
        << "Animated static preview built. Vertices for drawing: "
        << previewVertexCount
        << std::endl;
}

void AnimatedModel::DrawStaticPreview(
    Shader& shader
)
{
    if (!loaded)
        return;

    if (!previewReady)
        return;

    if (previewVAO == 0)
        return;

    shader.use();

    glBindVertexArray(
        previewVAO
    );

    glDrawArrays(
        GL_TRIANGLES,
        0,
        previewVertexCount
    );

    glBindVertexArray(
        0
    );
}
void AnimatedModel::Load(
    const std::string& path
)
{
    sourcePath =
        path;

    loaded =
        false;
    previewVertexData.clear();

    previewVertexCount =
        0;

    previewReady =
        false;
    vertices.clear();
    boneInfoMap.clear();
    animationNames.clear();

    boneCounter =
        0;

    meshCount =
        0;

    animationCount =
        0;

    std::cout
        << "Trying to load animated FBX: "
        << path
        << std::endl;

    std::ifstream fileCheck(
        path
    );

    if (!fileCheck.good())
    {
        std::cout
            << "ANIMATION FBX FILE NOT FOUND: "
            << path
            << std::endl;

        return;
    }

    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_LimitBoneWeights
        );

    if (!scene || !scene->mRootNode)
    {
        std::cout
            << "Animated model load failed: "
            << importer.GetErrorString()
            << std::endl;

        return;
    }

    meshCount =
        static_cast<int>(
            scene->mNumMeshes
            );

    animationCount =
        static_cast<int>(
            scene->mNumAnimations
            );

    std::cout
        << "Animated model loaded: "
        << path
        << std::endl;

    std::cout
        << "Meshes: "
        << meshCount
        << std::endl;

    std::cout
        << "Animations: "
        << animationCount
        << std::endl;

    ProcessNode(
        scene->mRootNode,
        scene
    );

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* animation =
            scene->mAnimations[i];

        std::string animationName =
            animation->mName.C_Str();

        if (animationName.empty())
        {
            animationName =
                "Animation_" +
                std::to_string(
                    i
                );
        }

        animationNames.push_back(
            animationName
        );

        std::cout
            << "Animation "
            << i
            << ": "
            << animationName
            << std::endl;

        std::cout
            << "Duration: "
            << animation->mDuration
            << std::endl;

        std::cout
            << "Ticks Per Second: "
            << animation->mTicksPerSecond
            << std::endl;

        std::cout
            << "Channels: "
            << animation->mNumChannels
            << std::endl;
    }

    std::cout
        << "Total animated vertices: "
        << vertices.size()
        << std::endl;

    std::cout
        << "Total bones found: "
        << boneCounter
        << std::endl;
    BuildStaticPreviewMesh();
    loaded =
        true;
}

void AnimatedModel::ProcessNode(
    aiNode* node,
    const aiScene* scene
)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh =
            scene->mMeshes[node->mMeshes[i]];

        ProcessMesh(
            mesh,
            scene
        );
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(
            node->mChildren[i],
            scene
        );
    }
}

void AnimatedModel::ProcessMesh(
    aiMesh* mesh,
    const aiScene* scene
)
{
    unsigned int vertexStartIndex =
        vertices.size();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        AnimatedVertex vertex;

        vertex.position =
            glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

        if (mesh->HasNormals())
        {
            vertex.normal =
                glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
        }
        else
        {
            vertex.normal =
                glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords =
                glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
        }
        else
        {
            vertex.texCoords =
                glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    ExtractBoneWeightForVertices(
        mesh,
        scene,
        vertexStartIndex
    );
    for (unsigned int faceIndex = 0;
        faceIndex < mesh->mNumFaces;
        faceIndex++)
    {
        aiFace face =
            mesh->mFaces[faceIndex];

        if (face.mNumIndices != 3)
            continue;

        for (unsigned int index = 0;
            index < face.mNumIndices;
            index++)
        {
            unsigned int vertexIndex =
                vertexStartIndex +
                face.mIndices[index];

            if (vertexIndex < vertices.size())
            {
                AddPreviewVertex(
                    vertices[vertexIndex]
                );
            }
        }
    }
    std::cout
        << "Processed mesh vertices: "
        << mesh->mNumVertices
        << " starting at "
        << vertexStartIndex
        << std::endl;
}

void AnimatedModel::ExtractBoneWeightForVertices(
    aiMesh* mesh,
    const aiScene* scene,
    unsigned int vertexStartIndex
)
{
    for (unsigned int boneIndex = 0;
        boneIndex < mesh->mNumBones;
        boneIndex++)
    {
        int boneID = -1;

        std::string boneName =
            mesh->mBones[boneIndex]->mName.C_Str();

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;

            newBoneInfo.id =
                boneCounter;

            aiMatrix4x4 aiOffset =
                mesh->mBones[boneIndex]->mOffsetMatrix;

            newBoneInfo.offset =
                glm::transpose(
                    glm::make_mat4(
                        &aiOffset.a1
                    )
                );

            boneInfoMap[boneName] =
                newBoneInfo;

            boneID =
                boneCounter;

            boneCounter++;

            std::cout
                << "New bone: "
                << boneName
                << " ID: "
                << boneID
                << std::endl;
        }
        else
        {
            boneID =
                boneInfoMap[boneName].id;
        }

        aiVertexWeight* weights =
            mesh->mBones[boneIndex]->mWeights;

        int numWeights =
            mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0;
            weightIndex < numWeights;
            weightIndex++)
        {
            int vertexID =
                static_cast<int>(
                    vertexStartIndex +
                    weights[weightIndex].mVertexId
                    );

            float weight =
                weights[weightIndex].mWeight;
            if (
                vertexID >= 0 &&
                vertexID < static_cast<int>(
                    vertices.size()
                    )
                )
            {
                SetVertexBoneData(
                    vertices[vertexID],
                    boneID,
                    weight
                );
            }
        }
    }
}

void AnimatedModel::SetVertexBoneData(
    AnimatedVertex& vertex,
    int boneID,
    float weight
)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (vertex.boneIDs[i] < 0)
        {
            vertex.boneIDs[i] =
                boneID;

            vertex.weights[i] =
                weight;

            return;
        }
    }
}