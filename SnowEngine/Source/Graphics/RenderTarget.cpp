#include "RenderTarget.h"

namespace Snow {
    RenderTarget::RenderTarget(Window* const window)
        : mWindow(window) {
        mSurface = Surface::Create(*mWindow);
    }

    RenderTarget::~RenderTarget() {
        delete mSurface;
    }

    Window* RenderTarget::GetWindow() { return mWindow; }

    Surface* RenderTarget::GetSurface() { return mSurface; }

    void RenderTarget::Begin() {
        mSurface->Begin();
    }

    void RenderTarget::End() {
        mSurface->End();
    }
}
