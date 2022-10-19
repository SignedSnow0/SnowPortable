#pragma once
#include <SnowEngine.h>
#include "EntityView.h"

class SceneView {
public:
    SceneView(EntityView* entityView);
    ~SceneView() = default;

    void SetProject(Snow::Project* project);
    
    void Draw();

private:
    Snow::Project* mProject{ nullptr };
    EntityView* mEntityView{ nullptr };
};