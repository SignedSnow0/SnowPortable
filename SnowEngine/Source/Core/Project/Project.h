#pragma once
#include <filesystem>
#include "Core/Common.h"
#include "Core/Scene.h"

namespace Snow {
    class Project {
    public:
        Project(const std::string& name, const std::filesystem::path& root);

        const std::string& Name() const;
        const std::filesystem::path& Root() const;
        const std::vector<Scene>& Scenes() const;
        const Scene* ActiveScene() const;

    private:
        std::string mName;
        std::filesystem::path mRoot;

        std::vector<Scene> mScenes;
        Scene* mActiveScene;
    };
}