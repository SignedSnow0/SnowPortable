#include "Application.h"
#include "Logger.h"
#include "Graphics/RenderTarget.h"
#include "Project/ProjectSerializer.h"

namespace Snow {
    Application* Application::sInstance{ nullptr };
    
    Application::Application(const AppInfo& info) {
        Logger::Initialize();
        Window::Initialize();
        Graphics::Initialize(info.Api);

        LOG_INFO("Application created");

        mRenderTarget = new RenderTarget(new Window(info.WindowTitle, info.WindowWidth, info.WindowHeight));
        mSceneRenderer = new SceneRenderer(*mRenderTarget);

        if (info.InitGui) {
            mGuiLayer = GuiLayer::Create(*mRenderTarget, mSceneRenderer->OutputRenderPass());
            mGuiLayer->DarkTheme();
        }

        sInstance = this;
    }

    Application::~Application() {
        delete mSceneRenderer;

        if (mGuiLayer)
            delete mGuiLayer;

        delete mRenderTarget;

        Graphics::Shutdown();
        Window::Shutdown();
        Logger::Shutdown();
    }

    Project* Application::CurrentProject() { return mProject; }

    void Application::SetProject(Project* project) {
        mProject = project;
        mSceneRenderer->SetScene(project->ActiveScene());
    }

    void Application::Run() {
        while (true) {
            Window::PollEvents();

            if (mRenderTarget->GetWindow()->ShouldClose()) {
                delete mRenderTarget->GetWindow();
                return;
            }

            mSceneRenderer->Update();
            
            mRenderTarget->Begin();

            mSceneRenderer->Render();

            if (mGuiLayer) {
                mGuiLayer->Begin();

                DrawGui();
                
                mGuiLayer->End();
            }

            mRenderTarget->End();
        }
    }

    Application* Application::GetInstance() { return sInstance; }
}