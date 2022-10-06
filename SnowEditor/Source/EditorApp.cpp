#include "EditorApp.h"


EditorApplication::EditorApplication()
    : Application({ "Snow Editor", 1920, 1080, Snow::GraphicsAPI::Vulkan, true }),
      mAssetsView{ std::filesystem::current_path() } {

}

EditorApplication::~EditorApplication() {

}

void EditorApplication::DrawGui() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Scene")) {
        
    }
    ImGui::End();

    mAssetsView.Draw();

    if (ImGui::Begin("View")) {
        
    }
    ImGui::End();
}