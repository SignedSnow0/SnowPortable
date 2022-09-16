#pragma once 
#include "Core/Common.h"

namespace Snow {
    enum class GraphicsAPI : u32 {
        Vulkan = 1
    };
    
    class Graphics {
    public:
        static b8 Initialize(const GraphicsAPI& api);
        static void Shutdown();

    protected:
        Graphics() = default;
        virtual ~Graphics() = default;
        
    private:
        static Graphics* sInstance;
    };
}