#pragma once
#include <filesystem>
#include "Core/Common.h"
#include "Core/Scene.h"

namespace Snow {
    class Project {
    public:
        Project(const std::string& name, const std::filesystem::path& root);

        const std::string& Name() const;
        const std::filesystem::path& RootFolder() const;
        std::vector<Scene*>& Scenes();
        Scene* ActiveScene();
        
        void AddScene(Scene* scene);
        void SetActiveScene(Scene* scene);

    private:
        std::string mName;
        std::filesystem::path mRootFolder;

        std::vector<Scene*> mScenes;
        Scene* mActiveScene{ nullptr };
    };
}