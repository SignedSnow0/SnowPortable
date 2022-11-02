#pragma once
#include "AssetsView.h"
#include "EntityView.h"
#include "SceneView.h"
#include "ResourcesView.h"

class EditorApplication : public Snow::Application {
public:
    EditorApplication();
    virtual ~EditorApplication() override;

    virtual void DrawGui() override;

    static EditorApplication* GetInstance();

private:
    AssetsView mAssetsView;
    EntityView mEntityView;
    SceneView mSceneView;
    ResourcesView mResourcesView;

    static EditorApplication* sInstance;
};