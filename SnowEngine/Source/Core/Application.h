#pragma once
#include "Common.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/Rhi/GuiLayer.h"

namespace Snow {
    struct AppInfo {
        const char* WindowTitle;
        u32 WindowWidth;
        u32 WindowHeight;

        GraphicsAPI Api;
        b8 InitGui{ false };
    };
    
    class Application {
    public:
        Application(const AppInfo& info);
        virtual ~Application();
        
        void Run();

    private:
        RenderTarget* mRenderTarget;
        GuiLayer* mGuiLayer;
    };
}