#pragma once
#include "Core/Common.h"
#include "Surface.h"

namespace Snow {
    struct RenderPassCreateInfo {
        u32 InitialWidth;
        u32 InitialHeight;
        u32 ImageCount;
        Surface* SurfaceOutput;
    };
    
    class RenderPass {
    public:
        static RenderPass* Create(const RenderPassCreateInfo& createInfo);
        virtual ~RenderPass() = default;

        virtual u32 Width() const = 0;
        virtual u32 Height() const = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

    protected:
        RenderPass() = default;
    };
}