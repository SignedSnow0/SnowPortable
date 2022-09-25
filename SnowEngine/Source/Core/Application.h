#pragma once
#include "Graphics/RenderTarget.h"
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
        RenderTarget* mRenderTarget;
    };
}