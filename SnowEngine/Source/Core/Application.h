#pragma once
#include "Platform/Window.h"
#include "Graphics/GraphicsCore.h"

namespace Snow {
    struct AppInfo {
        const char* WindowTitle;
        u32 WindowWidth;
        u32 WindowHeight;

        GraphicsAPI Api;
    };
    
    class Application {
    public:
        Application(const AppInfo& info);
        virtual ~Application();
        
        void Run();

    private:
        Window* mWindow{ nullptr };
    };
}