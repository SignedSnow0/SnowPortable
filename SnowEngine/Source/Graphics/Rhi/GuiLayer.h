#pragma once
#include "Graphics/RenderTarget.h"
#include "Graphics/Rhi/RenderPass.h"

namespace Snow {
    class GuiLayer {
    public:
        static GuiLayer* Create(const RenderTarget& rt, RenderPass* scene);
        virtual ~GuiLayer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        void DarkTheme() const;
    };
}