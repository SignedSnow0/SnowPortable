#include "EditorApp.h"
#include "Controls.h"

EditorApplication::EditorApplication()
    : Application({ "Snow Editor", 1920, 1080, Snow::GraphicsAPI::Vulkan, true }),
      mAssetsView{ std::filesystem::current_path() },
      mEntityView{ },
      mSceneView{ &mEntityView } {

    mSceneView.SetScene(ActiveScene());
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

    mSceneView.Draw();

    mAssetsView.Draw();

    mEntityView.Draw();
}