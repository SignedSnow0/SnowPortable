#include "SceneView.h"

SceneView::SceneView(EntityView* entityView)
    : mEntityView(entityView) { }

void SceneView::SetScene(Snow::Scene* scene) { mScene = scene; }

void SceneView::Draw() {
    if (ImGui::Begin("Scene") && mScene) {
        if (ImGui::Button("Add entity")) {
            mScene->CreateEntity();
        }
        ImGui::Separator();

        mScene->ExecuteForEachEntity([&](Snow::Entity& entity) {
            ImGui::PushID(entity.Id());
            std::string name{ "Entity " };
            name.append(std::to_string(entity.Id()));
            
            if (ImGui::Selectable(name.c_str(), mEntityView->ActiveEntity().Id() == entity.Id())) {
                mEntityView->SetEntity(entity);
            }
            
            ImGui::PopID();
        });
    }
    ImGui::End();
}