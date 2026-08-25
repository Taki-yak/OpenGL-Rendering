#pragma once

#include <string>
#include <map>
#include <iostream>

#include "AnimatedModel.h"

class AnimationLibrary
{
public:
    std::map<std::string, AnimatedModel*> animations;

    ~AnimationLibrary()
    {
        for (auto& pair : animations)
        {
            delete pair.second;
        }

        animations.clear();
    }

    void LoadAnimation(
        const std::string& name,
        const std::string& path
    )
    {
        if (animations.find(name) != animations.end())
        {
            delete animations[name];
            animations[name] =
                nullptr;
        }

        AnimatedModel* model =
            new AnimatedModel(
                path
            );

        animations[name] =
            model;

        std::cout
            << "Loaded animation clip request: "
            << name
            << std::endl;

        if (model->IsLoaded())
        {
            std::cout
                << "Clip OK: "
                << name
                << " | Animations: "
                << model->GetAnimationCount()
                << " | Bones: "
                << model->GetBoneCount()
                << " | Vertices: "
                << model->GetVertexCount()
                << std::endl;
        }
        else
        {
            std::cout
                << "Clip FAILED: "
                << name
                << std::endl;
        }
    }

    AnimatedModel* GetAnimation(
        const std::string& name
    )
    {
        if (animations.find(name) != animations.end())
        {
            return animations[name];
        }

        return nullptr;
    }

    void PrintSummary()
    {
        std::cout
            << "========== ANIMATION LIBRARY SUMMARY =========="
            << std::endl;

        for (auto& pair : animations)
        {
            AnimatedModel* model =
                pair.second;

            if (model == nullptr)
                continue;

            std::cout
                << pair.first
                << " | Loaded: "
                << model->IsLoaded()
                << " | Meshes: "
                << model->GetMeshCount()
                << " | Animations: "
                << model->GetAnimationCount()
                << " | Bones: "
                << model->GetBoneCount()
                << " | Vertices: "
                << model->GetVertexCount()
                << std::endl;
        }

        std::cout
            << "==============================================="
            << std::endl;
    }
};