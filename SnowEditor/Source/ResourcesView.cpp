#include "ResourcesView.h"
#include <SnowEngine.h>

void ResourcesView::Draw() {
    if (ImGui::Begin("Resources")) {
        ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Shaders")) {
            for (const auto& [id, shader] : Snow::Importer::ShaderCache()) {
                ImGui::Selectable(shader->Name().c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    shader->Reload();
                }

                if (ImGui::TreeNode("Layout")) {
                    for (const auto& [set, layout] : shader->Layouts()) {
                        ImGui::Text("Set: %d", set);
                        ImGui::Indent();
                        for (const auto& [binding, resource] : layout->Resources()) {
                            ImGui::Text("Binding: %d", binding);
                            ImGui::Text("Name: %s", resource->Name.c_str());
                            ImGui::Text("Size: %d", resource->Size);
                            ImGui::Text("Type: %s", resource->Type == Snow::ShaderResourceType::Uniform ? "Uniform" : "Image");
                        }
                        ImGui::Unindent();
                        ImGui::NewLine();
                    }
                    ImGui::TreePop();
                }
                ImGui::Separator();
            }
            
            ImGui::TreePop();
        }
    }
    ImGui::End();
}