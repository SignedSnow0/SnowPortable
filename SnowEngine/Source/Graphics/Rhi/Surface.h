#pragma once
#include "Platform/Window.h"

namespace Snow {
    class Surface {
    public:
        static Surface* Create(const Window& window);
        virtual ~Surface() = default;

        virtual u32 Width() const = 0;
        virtual u32 Height() const = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

    protected:
        Surface() = default;
    };
}
