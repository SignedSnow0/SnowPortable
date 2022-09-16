#include "Application.h"
#include "Logger.h"

namespace Snow {
    Application::Application(const AppInfo& info) {
        Logger::Initialize();
        Window::Initialize();
        Graphics::Initialize(info.Api);

        LOG_INFO("Application created");

        mWindow = new Window(info.WindowTitle, info.WindowWidth, info.WindowHeight);
    }

    Application::~Application() {
        Graphics::Shutdown();
        Window::Shutdown();
        Logger::Shutdown();
    }
    
    void Application::Run() {
        while (mWindow) {
            Window::PollEvents();

            if (mWindow->ShouldClose()) {
                delete mWindow;
                mWindow = nullptr;
            }
        }
    }
}