#include "Application.h"
#include "Logger.h"
#include "Graphics/Rhi/Surface.h"

namespace Snow {
    Application::Application(const AppInfo& info) {
        Logger::Initialize();
        Window::Initialize();
        Graphics::Initialize(info.Api);

        LOG_INFO("Application created");

        mRenderTarget = new RenderTarget(new Window(info.WindowTitle, info.WindowWidth, info.WindowHeight));

        Graphics::CreateDefaultResources(mRenderTarget->GetSurface());
    }

    Application::~Application() {
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

            Graphics::DebugDraw();
            
            mRenderTarget->End();
        }
    }
}