#include "AssetsView.h"
#include <imgui.h>
#include "Utils.h"
#include "Controls.h"
#include "EditorApp.h"

AssetsView::AssetsView(const std::filesystem::path& initialPath)
    : mCurrentPath{ initialPath } {
    mFolderIcon = Snow::Image::Create({ Snow::ImageUsage::Image, Utils::GetImagesPath() / "Folder-Icon.png" });
    mMeshIcon = Snow::Image::Create({ Snow::ImageUsage::Image, Utils::GetImagesPath() / "Mesh.png" });
    mImageIcon = Snow::Image::Create({ Snow::ImageUsage::Image, Utils::GetImagesPath() / "Image.png" });
}

AssetsView::~AssetsView() {
}

void AssetsView::SetProject(Snow::Project* project) {
    mProject = project;
    mCurrentPath = mProject->RootFolder();
}

void AssetsView::Draw() {
    if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse) && mProject) {
        { //Folder location
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f,0.0f,0.0f,0.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0,0 });
            auto path = mProject->RootFolder();
            while (path <= mCurrentPath) {
                if (ImGui::Button(path.filename().string().c_str())) {
                    mCurrentPath = path;
                }
                ImGui::SameLine();
                ImGui::Text("/");
                ImGui::SameLine();

                auto diff = std::filesystem::path(mCurrentPath.string().erase(0, path.string().size() + 1));
				path /=  diff.string().substr(0, diff.string().find_first_of('\\'));
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
            ImGui::NewLine();
        }
        ImGui::Separator();
        ImGui::BeginTable("AssetsView", 2, ImGuiTableFlags_Resizable, ImGui::GetContentRegionAvail());
        { //Important folders
            ImGui::TableNextColumn();
            ImGui::BeginChild("Important files");

            const std::filesystem::path& root{ mProject->RootFolder() };
            for (auto& item : std::filesystem::directory_iterator(root)) {
                if (!item.is_directory()) {
                    continue;
                }

                if (item.path().filename() == "Assets") {
                    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver); 
                    if (ImGui::TreeNode("Assets")) {
                        for (auto& item : std::filesystem::directory_iterator(item.path())) {
                            if (!item.is_directory()) {
                                continue;
                            }

                            if (item.path().filename() == "Models") {
                                if (ImGui::Selectable("Models")) {
                                    mCurrentPath = item.path();
                                }
                            }
                            else if (item.path().filename() == "Textures") {
                                if (ImGui::Selectable("Textures")) {
                                    mCurrentPath = item.path();
                                }
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::TableNextColumn();
		
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x * 1.5f, ImGui::GetStyle().ItemSpacing.y * 2.0f });
        ImGui::BeginChild("All files");
        for (auto& item : std::filesystem::directory_iterator(mCurrentPath)) {
            if (ImGui::GetContentRegionAvail().x < 110.0f) {
                ImGui::NewLine();
            }

            const auto& path = item.path();
            if (item.is_directory()) {
                ImGui::TextImageButton(path.filename().string(), mFolderIcon->GuiId(), { 100, 100 });
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    mCurrentPath = item.path();
                }
            }
            else if (item.path().extension() == ".gltf") {
                ImGui::TextImageButton(path.filename().string(), mMeshIcon->GuiId(), { 100, 100 });
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    
                }

                if (ImGui::BeginDragDropSource()) {
					ImGui::SetDragDropPayload("meshFile", path.string().c_str(), path.string().size() + 1, ImGuiCond_Once);
					
                    ImGui::EndDragDropSource();
                }
            }
            else if (item.path().extension() == ".jpg" || item.path().extension() == ".png" || item.path().extension() == ".jpeg") {
                ImGui::TextImageButton(path.filename().string(), mImageIcon->GuiId(), { 100, 100 });
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    
                }

                if (ImGui::BeginDragDropSource()) {
					ImGui::SetDragDropPayload("imageFile", path.string().c_str(), path.string().size() + 1, ImGuiCond_Once);
					
                    ImGui::EndDragDropSource();
                }
            }
            ImGui::SameLine();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::EndTable();
    }
    ImGui::End();
}