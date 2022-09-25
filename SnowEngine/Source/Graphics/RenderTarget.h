#pragma once
#include "Core/Common.h"
#include "Platform/Window.h"
#include "Graphics/Rhi/Surface.h"

namespace Snow {
    class RenderTarget {
    public:
        RenderTarget() = default;
        RenderTarget(Window* const window);
        ~RenderTarget();

        void Begin();
        void End();

        Window* GetWindow();
        Surface* GetSurface();

    private:
        Window* mWindow{ nullptr };
        Surface* mSurface{ nullptr };
    };
}