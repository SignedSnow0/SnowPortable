#pragma once 
#include "Core/Common.h"
#include "Graphics/Rhi/Shader.h"
#include "Graphics/Rhi/RenderPass.h"
#include "Graphics/Rhi/Pipeline.h"
#include "Graphics/Rhi/Surface.h"

namespace Snow {
    enum class GraphicsAPI : u32 {
        Vulkan = 1
    };
    
    class Graphics {
    public:
        static b8 Initialize(const GraphicsAPI& api);
        static void Shutdown();

        static GraphicsAPI Api();

        static void CreateDefaultResources(Surface* surface);
        static void DebugDraw();

    protected:
        Graphics() = default;
        virtual ~Graphics() = default;
        
    private:
        struct GraphicsResources {
            Shader* ShaderProgram;
            RenderPass* TargetPass;
            Pipeline* GraphicsPipeline;
        } mDefaultResources;

        GraphicsAPI mApi;
        
        static Graphics* sInstance;
    };
}