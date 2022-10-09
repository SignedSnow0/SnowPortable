#pragma once
#include "Core/Common.h"
#include "Surface.h"
#include "Image.h"

namespace Snow {
    enum class RenderPassUsage : u32 {
        Presentation,
        Image
    };
    
    struct RenderPassCreateInfo {
        RenderPassUsage Usage;
        u32 InitialWidth;
        u32 InitialHeight;
        u32 ImageCount;
        Surface* SurfaceOutput;
        b8 DepthAttachment;
    };
    
    class RenderPass {
    public:
        static RenderPass* Create(const RenderPassCreateInfo& createInfo);
        virtual ~RenderPass() = default;

        virtual u32 Width() const = 0;
        virtual u32 Height() const = 0;
        virtual const std::vector<Image*>& Images() const = 0;
        virtual b8 HasDepth() const = 0;
        
        virtual void Begin() = 0;
        virtual void End() = 0;

    protected:
        RenderPass() = default;
    };
}