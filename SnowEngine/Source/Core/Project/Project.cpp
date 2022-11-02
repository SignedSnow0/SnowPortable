#include "Project.h"
#include "Core/Application.h"

namespace Snow {
    Project::Project(const std::string& name, const std::filesystem::path& root)
        : mName{ name }, mRootFolder{ root } { }
	
    Project::~Project() {
        for (auto* scene : mScenes) {
            delete scene;
        }
    }

    const std::string& Project::Name() const { return mName; }

    const std::filesystem::path& Project::RootFolder() const { return mRootFolder; }

    std::vector<Scene*>& Project::Scenes() { return mScenes; }

    Scene* Project::ActiveScene() { return mActiveScene; }

    void Project::AddScene(Scene* scene) { mScenes.push_back(scene); }

    void Project::SetActiveScene(Scene* scene) {
        mActiveScene = scene;
        Application::GetInstance()->mSceneRenderer->SetScene(scene);
    }
}