#include "Application.h"
#include "Logger.h"

namespace Snow {
    Application::Application() {
        Logger::Initialize();
        Window::Initialize();

        LOG_INFO("Application created");

        mWindow = new Window("SnowEngine", 1280, 720);
    }

    Application::~Application() {
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