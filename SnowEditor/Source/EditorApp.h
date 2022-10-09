#include <EntryPoint.h>
#include "AssetsView.h"
#include "EntityView.h"
#include "SceneView.h"

class EditorApplication : public Snow::Application {
public:
    EditorApplication();
    virtual ~EditorApplication() override;

    virtual void DrawGui() override;

private:
    AssetsView mAssetsView;
    EntityView mEntityView;
    SceneView mSceneView;
};

void CreateApplication(Snow::Application*& app) {
    app = new EditorApplication();
}

void DestroyApplication(Snow::Application* app) {
    delete app;
}