#pragma once
#include "Common.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/Rhi/GuiLayer.h"
#include "Scene.h"
#include "Graphics/SceneRenderer.h"
#include "Project/Project.h"

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
        
        Project* CurrentProject();
        void SetProject(Project* project);

        void Run();

        static Application* GetInstance();

    protected:
        virtual void DrawGui() { }

    private:
        RenderTarget* mRenderTarget;
        GuiLayer* mGuiLayer;
        SceneRenderer* mSceneRenderer;
        Project* mProject{ nullptr };

        static Application* sInstance;
        
        friend class Project;
    };
}