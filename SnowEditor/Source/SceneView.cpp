#include "SceneView.h"

SceneView::SceneView(EntityView* entityView)
    : mEntityView{ entityView } { }

void SceneView::SetProject(Snow::Project* project) { mProject = project; }

void SceneView::Draw() {
    if (ImGui::Begin("Scene") && mProject && mProject->ActiveScene()) {
        Snow::Scene* scene{ mProject->ActiveScene() };
        
        if (ImGui::BeginCombo("##Scene", scene->Name().c_str())) {
            u32 i{ 0 };
            for (auto* item : mProject->Scenes()) {
                ImGui::PushID(i);
                if (ImGui::Selectable(item->Name().c_str(), item == scene)) {
                    scene = item;
                    mProject->SetActiveScene(item);
                }
               
                ImGui::PopID();
                i++;
            }
            
            ImGui::EndCombo();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Add entity")) {
            scene->CreateEntity();
        }
        ImGui::Separator();

        scene->ExecuteForEachEntity([&](Snow::Entity entity) {
            ImGui::PushID(entity.Id());
            if (ImGui::Selectable(entity.GetComponent<Snow::TagComponent>().Tag.c_str(), mEntityView->ActiveEntity().Id() == entity.Id())) {
                mEntityView->SetEntity(entity);
            }
            
            ImGui::PopID();
        });
    }
    ImGui::End();
}