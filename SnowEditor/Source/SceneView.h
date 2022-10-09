#pragma once
#include <SnowEngine.h>
#include "EntityView.h"

class SceneView {
public:
    SceneView(EntityView* entityView);
    ~SceneView() = default;

    void SetScene(Snow::Scene* scene);
    
    void Draw();

private:
    Snow::Scene* mScene{ nullptr };
    EntityView* mEntityView{ nullptr };
};