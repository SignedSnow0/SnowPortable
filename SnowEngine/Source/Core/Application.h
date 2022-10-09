#pragma once
#include "Common.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/Rhi/GuiLayer.h"
#include "Scene.h"
#include "Graphics/SceneRenderer.h"

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

    protected:
        Scene* ActiveScene();
        
        virtual void DrawGui() { }

    private:
        RenderTarget* mRenderTarget;
        GuiLayer* mGuiLayer;
        Scene* mScene;
        SceneRenderer* mSceneRenderer;
    };
}