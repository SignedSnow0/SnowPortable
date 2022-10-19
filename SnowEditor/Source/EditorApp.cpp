#include "EditorApp.h"
#include <EntryPoint.h>
#include "Controls.h"

static ImVec4 RGBAtoVec4(const vec4u& color) {
    return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

static ImVec4 RGBtoVec4(const vec3u& color) {
    return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f);
}

static void DarkTheme() {
    static vec3u darkElevations[]{
        { 18, 18, 18 },
        { 30, 30, 30 },
        { 34, 34, 34 },
        { 37, 37, 37 },
        { 39, 39, 39 },
        { 44, 44, 44 },
        { 46, 46, 46 },
        { 51, 51, 51 },
        { 53, 53, 53 },
        { 56, 56, 56 }
    };

    ImVec4* colors{ ImGui::GetStyle().Colors };
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = RGBtoVec4(darkElevations[0]);
    colors[ImGuiCol_ChildBg] = RGBtoVec4(darkElevations[0]);
    colors[ImGuiCol_PopupBg] = RGBtoVec4(darkElevations[0]);
    colors[ImGuiCol_Border] = RGBtoVec4(darkElevations[2]);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = RGBtoVec4(darkElevations[2]);
    colors[ImGuiCol_ButtonHovered] = RGBtoVec4(darkElevations[3]);
    colors[ImGuiCol_ButtonActive] = RGBtoVec4(darkElevations[4]);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = RGBtoVec4(darkElevations[2]);
    colors[ImGuiCol_SeparatorHovered] = RGBtoVec4(darkElevations[4]);
    colors[ImGuiCol_SeparatorActive] = RGBtoVec4(darkElevations[5]);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = RGBtoVec4(darkElevations[2]);
    colors[ImGuiCol_TabHovered] = RGBtoVec4(darkElevations[4]);
    colors[ImGuiCol_TabActive] = RGBtoVec4(darkElevations[5]);
    colors[ImGuiCol_TabUnfocused] = RGBtoVec4(darkElevations[2]);
    colors[ImGuiCol_TabUnfocusedActive] = RGBtoVec4(darkElevations[4]);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    ImGuiStyle style{ ImGui::GetStyle() };
    style.WindowMenuButtonPosition = ImGuiDir_Right;

    style.TabRounding = 0.0f;
    style.FrameRounding = 3.0f;

}


EditorApplication* EditorApplication::sInstance{ nullptr };

EditorApplication::EditorApplication()
    : Application({ "Snow Editor", 1920, 1080, Snow::GraphicsAPI::Vulkan, true }),
      mAssetsView{ std::filesystem::current_path() },
      mEntityView{ },
      mSceneView{ &mEntityView } {
    DarkTheme();

    sInstance = this;
}

EditorApplication::~EditorApplication() {

}

EditorApplication* EditorApplication::GetInstance() { return sInstance; }

void EditorApplication::DrawGui() {
    if (ImGui::BeginMainMenuBar()) {
        Snow::Project* project = CurrentProject();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Project", nullptr, nullptr, project)) {
                Snow::ProjectSerializer::Serialize(*project);
            }   
            if (ImGui::MenuItem("Open Project")) {
                std::filesystem::path projectPath;
                if (Snow::Utils::FileDialog(projectPath, std::filesystem::current_path(), { { "SnowEngine Project", "snow" } })) {
                    project = Snow::ProjectSerializer::Deserialize(projectPath);

                    Application::SetProject(project);
                    mSceneView.SetProject(project);
                    mAssetsView.SetProject(project);
                }
            }
            if (ImGui::MenuItem("New Project")) {
                std::filesystem::path projectPath;
                if (Snow::Utils::FolderDialog(projectPath, std::filesystem::current_path())) {
                    project = new Snow::Project(projectPath.filename().string(), projectPath);

                    Application::SetProject(project);
                    mSceneView.SetProject(project);
                    mAssetsView.SetProject(project);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Project", project)) {
            if (ImGui::MenuItem("Add scene")) {
                Snow::Scene* scene = new Snow::Scene();
                project->AddScene(scene);
                project->SetActiveScene(scene);
            }
            
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    mSceneView.Draw();

    mAssetsView.Draw();

    mEntityView.Draw();
}

void CreateApplication(Snow::Application*& app) {
    app = new EditorApplication();
}

void DestroyApplication(Snow::Application* app) {
    delete app;
}