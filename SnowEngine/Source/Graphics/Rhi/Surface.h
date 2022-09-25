#pragma once
#include "Platform/Window.h"

namespace Snow {
    class Surface {
    public:
        static Surface* Create(const Window& window);
        virtual ~Surface() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

    protected:
        Surface() = default;
    };
}
