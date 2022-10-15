#include "Project.h"

namespace Snow {
    Project::Project(const std::string& name, const std::filesystem::path& root)
        : mName{ name }, mRoot{ root } {

    }

    const std::string& Project::Name() const { return mName; }

    const std::filesystem::path& Project::Root() const { return mRoot; }

    const std::vector<Scene>& Project::Scenes() const { return mScenes; }

    const Scene* Project::ActiveScene() const { return mActiveScene; }
}