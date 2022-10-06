#pragma once 
#include "Core/Common.h"
#include "Graphics/Rhi/Shader.h"
#include "Graphics/Rhi/RenderPass.h"
#include "Graphics/Rhi/Pipeline.h"
#include "Graphics/Rhi/Surface.h"
#include "Graphics/Rhi/Buffers.h"
#include "Graphics/Rhi/DescriptorSet.h"

namespace Snow {
    enum class GraphicsAPI : u32 {
        Vulkan = 1
    };
    
    class Graphics {
    public:
        static b8 Initialize(const GraphicsAPI& api);
        static void Shutdown();

        static GraphicsAPI Api();

        static void DebugDraw();

    protected:
        Graphics() = default;
        virtual ~Graphics() = default;
        
    private:
        GraphicsAPI mApi;
        
        static Graphics* sInstance;
    };
}