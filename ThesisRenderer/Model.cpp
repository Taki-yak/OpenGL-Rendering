#include "Model.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cctype>

static bool ModelFileExists(
    const std::string& path
)
{
    std::ifstream file(
        path.c_str()
    );

    return file.good();
}

static std::string NormalizeModelPath(
    std::string path
)
{
    std::replace(
        path.begin(),
        path.end(),
        '\\',
        '/'
    );

    return path;
}

static std::string GetFileNameOnly(
    const std::string& path
)
{
    size_t slash =
        path.find_last_of(
            "/\\"
        );

    if (slash == std::string::npos)
    {
        return path;
    }

    return path.substr(
        slash + 1
    );
}

static std::string ToLowerText(
    std::string text
)
{
    std::transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c)
        {
            return (char)std::tolower(c);
        }
    );

    return text;
}

static bool ContainsText(
    const std::string& text,
    const std::string& word
)
{
    return ToLowerText(text).find(
        ToLowerText(word)
    ) != std::string::npos;
}

static bool IsMusicNpcTextureDirectory(
    const std::string& textureDirectory
)
{
    return
        ContainsText(textureDirectory, "Player") ||
        ContainsText(textureDirectory, "MusicNPC") ||
        ContainsText(textureDirectory, "SpeedDial");
}

static std::string ChooseMusicNpcFallbackTexture(
    const std::string& materialName,
    const std::string& meshName
)
{
    std::string combined =
        materialName + " " + meshName;

    if (ContainsText(combined, "head"))
    {
        return "textures/T_SpeedDial_Head_D.png";
    }

    if (
        ContainsText(combined, "face") ||
        ContainsText(combined, "acc")
        )
    {
        return "textures/T_SpeedDial_FaceAcc_D.png";
    }

    return "textures/T_SpeedDial_Body_D.png";
}
Model::Model(
    const std::string& path,
    const std::string& textureFolder
)
{
    textureDirectory =
        NormalizeModelPath(
            textureFolder
        );

    if (
        !textureDirectory.empty() &&
        textureDirectory.back() != '/'
        )
    {
        textureDirectory += "/";
    }

    LoadModel(
        path
    );
}

void Model::Draw(
    Shader& shader
)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        int textureIndex =
            -1;

        if (i < meshTextureIndices.size())
        {
            textureIndex =
                meshTextureIndices[i];
        }

        bool useMeshTexture =
            textureIndex >= 0 &&
            textureIndex < (int)loadedTextures.size();

        shader.setBool(
            "useTexture",
            useMeshTexture
        );

        if (useMeshTexture)
        {
            glActiveTexture(
                GL_TEXTURE0
            );

            loadedTextures[textureIndex].Bind();

            shader.setVec3(
                "materialTint",
                glm::vec3(
                    1.0f
                )
            );
        }
        else
        {
            if (i < meshDiffuseColors.size())
            {
                shader.setVec3(
                    "materialTint",
                    meshDiffuseColors[i]
                );
            }
            else
            {
                shader.setVec3(
                    "materialTint",
                    glm::vec3(
                        1.0f
                    )
                );
            }
        }

        meshes[i].Draw();
    }

    shader.setBool(
        "useTexture",
        true
    );

    shader.setVec3(
        "materialTint",
        glm::vec3(
            1.0f
        )
    );
}

void Model::LoadModel(
    std::string path
)
{
    Assimp::Importer importer;

    path =
        NormalizeModelPath(
            path
        );

    const aiScene* scene =
        importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace
        );

    if (
        !scene ||
        scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode
        )
    {
        std::cout
            << "Assimp error: "
            << importer.GetErrorString()
            << std::endl;

        return;
    }

    size_t slash =
        path.find_last_of(
            "/\\"
        );

    directory =
        slash == std::string::npos
        ? "."
        : path.substr(
            0,
            slash
        );

    directory =
        NormalizeModelPath(
            directory
        );

    std::cout
        << "Model directory: "
        << directory
        << std::endl;

    std::cout
        << "Texture directory: "
        << textureDirectory
        << std::endl;

    ProcessNode(
        scene->mRootNode,
        scene
    );
}

void Model::ProcessNode(
    aiNode* node,
    const aiScene* scene
)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh =
            scene->mMeshes[
                node->mMeshes[i]
            ];

        meshes.push_back(
            ProcessMesh(
                mesh,
                scene
            )
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

bool Model::TryGetMaterialTexture(
    aiMaterial* material,
    aiTextureType textureType,
    std::string& outputPath
)
{
    aiString texturePath;

    if (
        material->GetTexture(
            textureType,
            0,
            &texturePath
        ) == AI_SUCCESS
        )
    {
        outputPath =
            std::string(
                texturePath.C_Str()
            );

        return !outputPath.empty();
    }

    return false;
}

std::string Model::ResolveTexturePath(
    const std::string& texturePath
)
{
    std::string cleanPath =
        NormalizeModelPath(
            texturePath
        );

    std::string fileNameOnly =
        GetFileNameOnly(
            cleanPath
        );

    std::vector<std::string> candidates;

    candidates.push_back(
        cleanPath
    );

    if (!textureDirectory.empty())
    {
        candidates.push_back(
            textureDirectory + cleanPath
        );

        candidates.push_back(
            textureDirectory + fileNameOnly
        );

        candidates.push_back(
            textureDirectory + "textures/" + fileNameOnly
        );
    }

    candidates.push_back(
        directory + "/" + cleanPath
    );

    candidates.push_back(
        directory + "/" + fileNameOnly
    );

    candidates.push_back(
        directory + "/textures/" + fileNameOnly
    );

    for (const std::string& candidate : candidates)
    {
        std::string normalizedCandidate =
            NormalizeModelPath(
                candidate
            );

        if (
            ModelFileExists(
                normalizedCandidate
            )
            )
        {
            return normalizedCandidate;
        }
    }

    return cleanPath;
}

int Model::GetLoadedTextureIndex(
    const std::string& texturePath
)
{
    std::string resolvedPath =
        ResolveTexturePath(
            texturePath
        );

    resolvedPath =
        NormalizeModelPath(
            resolvedPath
        );

    for (unsigned int i = 0; i < loadedTexturePaths.size(); i++)
    {
        if (loadedTexturePaths[i] == resolvedPath)
        {
            return (int)i;
        }
    }

    std::cout
        << "Trying to load texture: "
        << resolvedPath
        << std::endl;

    if (
        !ModelFileExists(
            resolvedPath
        )
        )
    {
        std::cout
            << "Texture file not found: "
            << resolvedPath
            << std::endl;

        return -1;
    }

    Texture texture(
        resolvedPath.c_str()
    );

    loadedTextures.push_back(
        texture
    );

    loadedTexturePaths.push_back(
        resolvedPath
    );

    std::cout
        << "Texture loaded: "
        << resolvedPath
        << std::endl;

    return (int)loadedTextures.size() - 1;
}

Mesh Model::ProcessMesh(
    aiMesh* mesh,
    const aiScene* scene
)
{
    std::vector<float> vertices;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face =
            mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            unsigned int index =
                face.mIndices[j];

            vertices.push_back(
                mesh->mVertices[index].x
            );

            vertices.push_back(
                mesh->mVertices[index].y
            );

            vertices.push_back(
                mesh->mVertices[index].z
            );

            if (mesh->HasNormals())
            {
                vertices.push_back(
                    mesh->mNormals[index].x
                );

                vertices.push_back(
                    mesh->mNormals[index].y
                );

                vertices.push_back(
                    mesh->mNormals[index].z
                );
            }
            else
            {
                vertices.push_back(
                    0.0f
                );

                vertices.push_back(
                    0.0f
                );

                vertices.push_back(
                    0.0f
                );
            }

            if (mesh->mTextureCoords[0])
            {
                vertices.push_back(
                    mesh->mTextureCoords[0][index].x
                );

                vertices.push_back(
                    mesh->mTextureCoords[0][index].y
                );
            }
            else
            {
                vertices.push_back(
                    0.0f
                );

                vertices.push_back(
                    0.0f
                );
            }
        }
    }

    glm::vec3 diffuseColor =
        glm::vec3(
            1.0f
        );

    int textureIndex =
        -1;

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material =
            scene->mMaterials[
                mesh->mMaterialIndex
            ];
        aiString materialNameString;

        material->Get(
            AI_MATKEY_NAME,
            materialNameString
        );

        std::string materialName =
            std::string(
                materialNameString.C_Str()
            );

        std::string meshName =
            std::string(
                mesh->mName.C_Str()
            );

        std::cout
            << "Material name: "
            << materialName
            << " | Mesh name: "
            << meshName
            << std::endl;
        aiColor3D color(
            1.0f,
            1.0f,
            1.0f
        );

        if (
            material->Get(
                AI_MATKEY_COLOR_DIFFUSE,
                color
            ) == AI_SUCCESS
            )
        {
            diffuseColor =
                glm::vec3(
                    color.r,
                    color.g,
                    color.b
                );
        }

        std::string texturePath;

        if (
            TryGetMaterialTexture(
                material,
                aiTextureType_DIFFUSE,
                texturePath
            )
            )
        {
            std::cout
                << "Diffuse texture from material: "
                << texturePath
                << std::endl;

            textureIndex =
                GetLoadedTextureIndex(
                    texturePath
                );
        }
        if (
            textureIndex == -1 &&
            IsMusicNpcTextureDirectory(
                textureDirectory
            )
            )
        {
            std::string fallbackTexture =
                ChooseMusicNpcFallbackTexture(
                    materialName,
                    meshName
                );

            std::cout
                << "Music NPC fallback texture: "
                << fallbackTexture
                << std::endl;

            textureIndex =
                GetLoadedTextureIndex(
                    fallbackTexture
                );
        }
        else if (
            TryGetMaterialTexture(
                material,
                aiTextureType_UNKNOWN,
                texturePath
            )
            )
        {
            std::cout
                << "Unknown texture from material: "
                << texturePath
                << std::endl;

            textureIndex =
                GetLoadedTextureIndex(
                    texturePath
                );
        }
    }

    meshDiffuseColors.push_back(
        diffuseColor
    );

    meshTextureIndices.push_back(
        textureIndex
    );

    return Mesh(
        vertices.data(),
        static_cast<int>(
            vertices.size() * sizeof(float)
            )
    );
}