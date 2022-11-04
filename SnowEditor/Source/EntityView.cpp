#include "EntityView.h"
#include "Controls.h"
#include "EditorApp.h"

void EntityView::SetEntity(Snow::Entity entity) { mEntity = entity; }

Snow::Entity EntityView::ActiveEntity() const { return mEntity; }

void EntityView::Draw() {
    if (ImGui::Begin("View") && mEntity.Valid()) {
		auto& tag = mEntity.GetComponent<Snow::TagComponent>();
        char buf[256]{ 0 };
		strcpy_s(buf, tag.Tag.c_str());
		ImGui::InputText("##EntityName", buf, 256);
		tag.Tag = buf;

        ImGui::SameLine();
        if (ImGui::BeginCombo("Add component", "Add component", ImGuiComboFlags_NoArrowButton)) {
            if (!mEntity.HasComponent<Snow::MeshComponent>() && ImGui::Selectable("Mesh component")) {
                mEntity.AddComponent<Snow::MeshComponent>();
            }

            ImGui::EndCombo();
        }

        ImGui::Separator();

        if (auto* comp = mEntity.TryGetComponent<Snow::TransformComponent>(); comp) {
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode("Transform component")) {
                ImGui::Vec3fSlider("Position", comp->Position);
                ImGui::Vec3fSlider("Rotation", comp->Rotation);
                ImGui::Vec3fSlider("Scale", comp->Scale, { 1.0f, 1.0f, 1.0f });

                ImGui::TreePop();
				
                ImGui::Separator();
            }
        }
        
        if (auto* comp = mEntity.TryGetComponent<Snow::MeshComponent>(); comp) {
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode("Mesh component")) {
                ImGui::Text(comp->Mesh ? comp->Source.filename().string().c_str() : "No mesh");
                if (ImGui::BeginDragDropTarget()) {
                    const auto* payload{ ImGui::AcceptDragDropPayload("meshFile") };
					if (payload) {
                        std::filesystem::path file{ static_cast<const char*>(payload->Data) };

                        comp->Mesh = Snow::Importer::ImportMesh(file);
                        comp->Source = file;
					}

                    ImGui::EndDragDropTarget();
                }
                if (ImGui::TreeNode("Material")) {
                    auto& mat = comp->Mesh->GetMaterial();
                    ImGui::Text(mat->Name().c_str());
                    {
                        auto albedo = mat->GetAlbedo();
                        ImGui::Text("Albedo");
                        ImGui::Image(reinterpret_cast<void*>(albedo->GuiId()), { 128.0f, 128.0f });
                        if (ImGui::BeginDragDropTarget()) {
                            const auto* payload{ ImGui::AcceptDragDropPayload("imageFile") };
                            if (payload) {
                                std::filesystem::path file{ static_cast<const char*>(payload->Data) };

                                Snow::ImageCreateInfo info;
                                info.Usage = Snow::ImageUsage::Image;
                                info.File = file;
                                Snow::ResourcePtr<Snow::Image> image{ Snow::Image::Create(info) };
                                mat->SetAlbedo(image);
                            }
                            
                            ImGui::EndDragDropTarget();
                        }

                        std::string source{ albedo->Path().string() };
                        ImGui::Text("%s", source.size() ? source.c_str() : "No source path");
                    }
                    {
                        auto normal = mat->GetNormal();
                        ImGui::Text("Normal");
                        ImGui::Image(reinterpret_cast<void*>(normal->GuiId()), { 128.0f, 128.0f });
                        if (ImGui::BeginDragDropTarget()) {
                            const auto* payload{ ImGui::AcceptDragDropPayload("imageFile") };
                            if (payload) {
                                std::filesystem::path file{ static_cast<const char*>(payload->Data) };

                                Snow::ImageCreateInfo info;
                                info.Usage = Snow::ImageUsage::Image;
                                info.File = file;
                                Snow::ResourcePtr<Snow::Image> image{ Snow::Image::Create(info) };
                                mat->SetNormal(image);
                            }
                            
                            ImGui::EndDragDropTarget();
                        }

                        std::string source{ normal->Path().string() };
                        ImGui::Text("%s", source.size() ? source.c_str() : "No source path");
                    }
                    {
                        auto metallicRoughness = mat->GetMetallicRoughness();
                        ImGui::Text("Metallic roughness");
                        ImGui::Image(reinterpret_cast<void*>(metallicRoughness->GuiId()), { 128.0f, 128.0f });
                        if (ImGui::BeginDragDropTarget()) {
                            const auto* payload{ ImGui::AcceptDragDropPayload("imageFile") };
                            if (payload) {
                                std::filesystem::path file{ static_cast<const char*>(payload->Data) };

                                Snow::ImageCreateInfo info;
                                info.Usage = Snow::ImageUsage::Image;
                                info.File = file;
                                Snow::ResourcePtr<Snow::Image> image{ Snow::Image::Create(info) };
                                mat->SetMetallicRoughness(image);
                            }
                            
                            ImGui::EndDragDropTarget();
                        }

                        std::string source{ metallicRoughness->Path().string() };
                        ImGui::Text("%s", source.size() ? source.c_str() : "No source path");
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();

                ImGui::Separator();
            }   
        }
    }
    ImGui::End();
}