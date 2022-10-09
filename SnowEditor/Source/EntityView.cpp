#include "EntityView.h"
#include "Controls.h"

void EntityView::SetEntity(Snow::Entity entity) { mEntity = entity; }

Snow::Entity EntityView::ActiveEntity() const { return mEntity; }

void EntityView::Draw() {
    if (ImGui::Begin("View") && mEntity.Valid()) {
        ImGui::Text("Entity %u", mEntity.Id());
        ImGui::Separator();

        if (auto* comp = mEntity.TryGetComponent<Snow::TransformComponent>(); comp) {
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode("Transform component")) {
                ImGui::Vec3fSlider("Position", comp->Position);
                ImGui::Vec3fSlider("Rotation", comp->Rotation);
                ImGui::Vec3fSlider("Scale", comp->Scale, { 1.0f, 1.0f, 1.0f });

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}