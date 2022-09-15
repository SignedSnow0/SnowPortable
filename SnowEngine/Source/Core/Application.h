#pragma once
#include "Platform/Window.h"

namespace Snow {
    class Application {
    public:
        Application();
        virtual ~Application();
        
        void Run();

    private:
        Window* mWindow{ nullptr };
    };
}