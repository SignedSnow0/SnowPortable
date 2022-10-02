#pragma once
#include "Graphics/RenderTarget.h"

namespace Snow {
    class GuiLayer {
    public:
        static GuiLayer* Create(const RenderTarget& rt);
        virtual ~GuiLayer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
}