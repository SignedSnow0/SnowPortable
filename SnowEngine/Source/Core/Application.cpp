#include "Application.h"
#include "Logger.h"
#include "Graphics/RenderTarget.h"
#include "Entity.h"//TODO: Remove this

namespace Snow {
    Application::Application(const AppInfo& info) {
        Logger::Initialize();
        Window::Initialize();
        Graphics::Initialize(info.Api);

        LOG_INFO("Application created");

        mRenderTarget = new RenderTarget(new Window(info.WindowTitle, info.WindowWidth, info.WindowHeight));

        mScene = new Scene();
        mSceneRenderer = new SceneRenderer(mScene, *mRenderTarget);

        if (info.InitGui) {
            mGuiLayer = GuiLayer::Create(*mRenderTarget, mSceneRenderer->OutputRenderPass());
            mGuiLayer->DarkTheme();
        }
    }

    Application::~Application() {
        delete mSceneRenderer;
        delete mScene;

        if (mGuiLayer)
            delete mGuiLayer;

        delete mRenderTarget;

        Graphics::Shutdown();
        Window::Shutdown();
        Logger::Shutdown();
    }
    
    void Application::Run() {
        while (true) {
            Window::PollEvents();

            if (mRenderTarget->GetWindow()->ShouldClose()) {
                delete mRenderTarget->GetWindow();
                return;
            }

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

    Scene* Application::ActiveScene() { return mScene; }
}