#include "AnimatedModel.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "AnimatedModel.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <fstream>
#include <cmath>
#include <algorithm>
static glm::mat4 ConvertAssimpMatrixToGlm(
    const aiMatrix4x4& matrix
)
{
    glm::mat4 result;

    result[0][0] = matrix.a1;
    result[1][0] = matrix.a2;
    result[2][0] = matrix.a3;
    result[3][0] = matrix.a4;

    result[0][1] = matrix.b1;
    result[1][1] = matrix.b2;
    result[2][1] = matrix.b3;
    result[3][1] = matrix.b4;

    result[0][2] = matrix.c1;
    result[1][2] = matrix.c2;
    result[2][2] = matrix.c3;
    result[3][2] = matrix.c4;

    result[0][3] = matrix.d1;
    result[1][3] = matrix.d2;
    result[2][3] = matrix.d3;
    result[3][3] = matrix.d4;

    return result;
}
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
    previewTriangleVertices.push_back(
        vertex
    );
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
        GL_DYNAMIC_DRAW
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
    previewTriangleVertices.clear();
    finalBoneMatrices.clear();

    loadedScene =
        nullptr;

    animationTime =
        0.0f;

    animationDuration =
        0.0f;

    ticksPerSecond =
        25.0f;
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

    importer.FreeScene();

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
    loadedScene =
        scene;

    globalInverseTransform =
        glm::inverse(
            ConvertAssimpMatrixToGlm(
                scene->mRootNode->mTransformation
            )
        );
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
        if (i == 0)
        {
            animationDuration =
                static_cast<float>(
                    animation->mDuration
                    );

            ticksPerSecond =
                animation->mTicksPerSecond != 0.0 ?
                static_cast<float>(
                    animation->mTicksPerSecond
                    ) :
                25.0f;
        }
    }

    std::cout
        << "Total animated vertices: "
        << vertices.size()
        << std::endl;

    std::cout
        << "Total bones found: "
        << boneCounter
        << std::endl;
    finalBoneMatrices.resize(
        boneCounter,
        glm::mat4(
            1.0f
        )
    );
    BuildStaticPreviewMesh();
    UpdateAnimation(
        0.0f
    );
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
void AnimatedModel::Play(
    bool resetTime
)
{
    animationPlaying =
        true;

    if (resetTime)
    {
        animationTime =
            0.0f;
    }
}

void AnimatedModel::Pause()
{
    animationPlaying =
        false;
}

void AnimatedModel::SetLooping(
    bool value
)
{
    animationLooping =
        value;
}

void AnimatedModel::SetAnimationSpeed(
    float speed
)
{
    if (speed < 0.0f)
        speed =
        0.0f;

    animationSpeed =
        speed;
}

void AnimatedModel::UpdateAnimation(
    float deltaTime
)
{
    if (!loaded)
        return;

    if (loadedScene == nullptr)
        return;

    if (loadedScene->mNumAnimations == 0)
        return;

    aiAnimation* animation =
        loadedScene->mAnimations[0];

    if (animation == nullptr)
        return;

    if (animationDuration <= 0.0f)
    {
        animationDuration =
            static_cast<float>(
                animation->mDuration
                );
    }

    if (ticksPerSecond <= 0.0f)
    {
        ticksPerSecond =
            25.0f;
    }

    if (animationPlaying)
    {
        animationTime +=
            deltaTime *
            ticksPerSecond *
            animationSpeed;

        if (animationDuration > 0.0f)
        {
            if (animationLooping)
            {
                animationTime =
                    std::fmod(
                        animationTime,
                        animationDuration
                    );
            }
            else if (animationTime > animationDuration)
            {
                animationTime =
                    animationDuration;

                animationPlaying =
                    false;
            }
        }
    }

    if (
        finalBoneMatrices.size() <
        static_cast<size_t>(
            boneCounter
            )
        )
    {
        finalBoneMatrices.resize(
            boneCounter,
            glm::mat4(
                1.0f
            )
        );
    }

    CalculateBoneTransform(
        loadedScene->mRootNode,
        glm::mat4(
            1.0f
        )
    );

    UpdateSkinnedPreviewMesh();
}

const aiNodeAnim* AnimatedModel::FindNodeAnimationChannel(
    aiAnimation* animation,
    const std::string& nodeName
)
{
    if (animation == nullptr)
        return nullptr;

    for (unsigned int i = 0;
        i < animation->mNumChannels;
        i++)
    {
        const aiNodeAnim* channel =
            animation->mChannels[i];

        if (channel == nullptr)
            continue;

        std::string channelName =
            channel->mNodeName.C_Str();

        if (channelName == nodeName)
            return channel;
    }

    return nullptr;
}

int AnimatedModel::GetPositionIndex(
    float animationTime,
    const aiNodeAnim* channel
)
{
    for (unsigned int i = 0;
        i < channel->mNumPositionKeys - 1;
        i++)
    {
        if (
            animationTime <
            static_cast<float>(
                channel->mPositionKeys[i + 1].mTime
                )
            )
        {
            return static_cast<int>(
                i
                );
        }
    }

    return
        static_cast<int>(
            channel->mNumPositionKeys - 2
            );
}

int AnimatedModel::GetRotationIndex(
    float animationTime,
    const aiNodeAnim* channel
)
{
    for (unsigned int i = 0;
        i < channel->mNumRotationKeys - 1;
        i++)
    {
        if (
            animationTime <
            static_cast<float>(
                channel->mRotationKeys[i + 1].mTime
                )
            )
        {
            return static_cast<int>(
                i
                );
        }
    }

    return
        static_cast<int>(
            channel->mNumRotationKeys - 2
            );
}

int AnimatedModel::GetScaleIndex(
    float animationTime,
    const aiNodeAnim* channel
)
{
    for (unsigned int i = 0;
        i < channel->mNumScalingKeys - 1;
        i++)
    {
        if (
            animationTime <
            static_cast<float>(
                channel->mScalingKeys[i + 1].mTime
                )
            )
        {
            return static_cast<int>(
                i
                );
        }
    }

    return
        static_cast<int>(
            channel->mNumScalingKeys - 2
            );
}

glm::vec3 AnimatedModel::InterpolatePosition(
    float animationTime,
    const aiNodeAnim* channel
)
{
    if (channel->mNumPositionKeys == 1)
    {
        aiVector3D value =
            channel->mPositionKeys[0].mValue;

        return glm::vec3(
            value.x,
            value.y,
            value.z
        );
    }

    int index =
        GetPositionIndex(
            animationTime,
            channel
        );

    int nextIndex =
        index + 1;

    float startTime =
        static_cast<float>(
            channel->mPositionKeys[index].mTime
            );

    float endTime =
        static_cast<float>(
            channel->mPositionKeys[nextIndex].mTime
            );

    float factor =
        (animationTime - startTime) /
        (endTime - startTime);

    factor =
        glm::clamp(
            factor,
            0.0f,
            1.0f
        );

    aiVector3D start =
        channel->mPositionKeys[index].mValue;

    aiVector3D end =
        channel->mPositionKeys[nextIndex].mValue;

    glm::vec3 startPosition(
        start.x,
        start.y,
        start.z
    );

    glm::vec3 endPosition(
        end.x,
        end.y,
        end.z
    );

    return glm::mix(
        startPosition,
        endPosition,
        factor
    );
}

glm::quat AnimatedModel::InterpolateRotation(
    float animationTime,
    const aiNodeAnim* channel
)
{
    if (channel->mNumRotationKeys == 1)
    {
        aiQuaternion value =
            channel->mRotationKeys[0].mValue;

        return glm::normalize(
            glm::quat(
                value.w,
                value.x,
                value.y,
                value.z
            )
        );
    }

    int index =
        GetRotationIndex(
            animationTime,
            channel
        );

    int nextIndex =
        index + 1;

    float startTime =
        static_cast<float>(
            channel->mRotationKeys[index].mTime
            );

    float endTime =
        static_cast<float>(
            channel->mRotationKeys[nextIndex].mTime
            );

    float factor =
        (animationTime - startTime) /
        (endTime - startTime);

    factor =
        glm::clamp(
            factor,
            0.0f,
            1.0f
        );

    aiQuaternion start =
        channel->mRotationKeys[index].mValue;

    aiQuaternion end =
        channel->mRotationKeys[nextIndex].mValue;

    glm::quat startRotation(
        start.w,
        start.x,
        start.y,
        start.z
    );

    glm::quat endRotation(
        end.w,
        end.x,
        end.y,
        end.z
    );

    return glm::normalize(
        glm::slerp(
            startRotation,
            endRotation,
            factor
        )
    );
}

glm::vec3 AnimatedModel::InterpolateScale(
    float animationTime,
    const aiNodeAnim* channel
)
{
    if (channel->mNumScalingKeys == 1)
    {
        aiVector3D value =
            channel->mScalingKeys[0].mValue;

        return glm::vec3(
            value.x,
            value.y,
            value.z
        );
    }

    int index =
        GetScaleIndex(
            animationTime,
            channel
        );

    int nextIndex =
        index + 1;

    float startTime =
        static_cast<float>(
            channel->mScalingKeys[index].mTime
            );

    float endTime =
        static_cast<float>(
            channel->mScalingKeys[nextIndex].mTime
            );

    float factor =
        (animationTime - startTime) /
        (endTime - startTime);

    factor =
        glm::clamp(
            factor,
            0.0f,
            1.0f
        );

    aiVector3D start =
        channel->mScalingKeys[index].mValue;

    aiVector3D end =
        channel->mScalingKeys[nextIndex].mValue;

    glm::vec3 startScale(
        start.x,
        start.y,
        start.z
    );

    glm::vec3 endScale(
        end.x,
        end.y,
        end.z
    );

    return glm::mix(
        startScale,
        endScale,
        factor
    );
}

void AnimatedModel::CalculateBoneTransform(
    aiNode* node,
    const glm::mat4& parentTransform
)
{
    if (node == nullptr)
        return;

    std::string nodeName =
        node->mName.C_Str();

    glm::mat4 nodeTransform =
        ConvertAssimpMatrixToGlm(
            node->mTransformation
        );

    aiAnimation* animation =
        loadedScene->mAnimations[0];

    const aiNodeAnim* channel =
        FindNodeAnimationChannel(
            animation,
            nodeName
        );

    if (channel != nullptr)
    {
        glm::vec3 translation =
            InterpolatePosition(
                animationTime,
                channel
            );

        glm::quat rotation =
            InterpolateRotation(
                animationTime,
                channel
            );

        glm::vec3 scale =
            InterpolateScale(
                animationTime,
                channel
            );

        glm::mat4 translationMatrix =
            glm::translate(
                glm::mat4(
                    1.0f
                ),
                translation
            );

        glm::mat4 rotationMatrix =
            glm::mat4_cast(
                rotation
            );
        glm::mat4 scaleMatrix =
            glm::scale(
                glm::mat4(
                    1.0f
                ),
                scale
            );

        nodeTransform =
            translationMatrix *
            rotationMatrix *
            scaleMatrix;
    }

    glm::mat4 globalTransform =
        parentTransform *
        nodeTransform;

    auto boneIterator =
        boneInfoMap.find(
            nodeName
        );

    if (boneIterator != boneInfoMap.end())
    {
        int boneID =
            boneIterator->second.id;

        if (
            boneID >= 0 &&
            boneID < static_cast<int>(
                finalBoneMatrices.size()
                )
            )
        {
            finalBoneMatrices[boneID] =
                globalInverseTransform *
                globalTransform *
                boneIterator->second.offset;
        }
    }

    for (unsigned int i = 0;
        i < node->mNumChildren;
        i++)
    {
        CalculateBoneTransform(
            node->mChildren[i],
            globalTransform
        );
    }
}

void AnimatedModel::UpdateSkinnedPreviewMesh()
{
    if (!previewReady)
        return;

    if (previewVBO == 0)
        return;

    if (previewTriangleVertices.empty())
        return;

    previewVertexData.clear();

    for (const AnimatedVertex& vertex : previewTriangleVertices)
    {
        glm::vec4 skinnedPosition =
            glm::vec4(
                0.0f
            );

        glm::vec3 skinnedNormal =
            glm::vec3(
                0.0f
            );

        float totalWeight =
            0.0f;

        for (int i = 0;
            i < MAX_BONE_INFLUENCE;
            i++)
        {
            int boneID =
                vertex.boneIDs[i];

            float weight =
                vertex.weights[i];

            if (
                boneID < 0 ||
                weight <= 0.0f ||
                boneID >= static_cast<int>(
                    finalBoneMatrices.size()
                    )
                )
            {
                continue;
            }

            glm::mat4 boneMatrix =
                finalBoneMatrices[boneID];

            skinnedPosition +=
                boneMatrix *
                glm::vec4(
                    vertex.position,
                    1.0f
                ) *
                weight;

            skinnedNormal +=
                glm::mat3(
                    boneMatrix
                ) *
                vertex.normal *
                weight;

            totalWeight +=
                weight;
        }

        if (totalWeight <= 0.0f)
        {
            skinnedPosition =
                glm::vec4(
                    vertex.position,
                    1.0f
                );

            skinnedNormal =
                vertex.normal;
        }

        if (glm::length(skinnedNormal) > 0.001f)
        {
            skinnedNormal =
                glm::normalize(
                    skinnedNormal
                );
        }
        else
        {
            skinnedNormal =
                glm::vec3(
                    0.0f,
                    1.0f,
                    0.0f
                );
        }

        previewVertexData.push_back(
            skinnedPosition.x
        );

        previewVertexData.push_back(
            skinnedPosition.y
        );

        previewVertexData.push_back(
            skinnedPosition.z
        );

        previewVertexData.push_back(
            skinnedNormal.x
        );

        previewVertexData.push_back(
            skinnedNormal.y
        );

        previewVertexData.push_back(
            skinnedNormal.z
        );

        previewVertexData.push_back(
            vertex.texCoords.x
        );

        previewVertexData.push_back(
            vertex.texCoords.y
        );
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        previewVBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        previewVertexData.size() *
        sizeof(float),
        previewVertexData.data()
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );
}