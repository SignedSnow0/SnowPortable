#include "Core/Common.h"

namespace Snow {
    class Window {
    public:
        Window(const std::string& title, u32 width, u32 height, b8 visible = true);
        ~Window();

        decltype(auto) Native();
        b8 ShouldClose();

        static b8 Initialize();
        static void Shutdown();

        static void PollEvents();

    private:
        Window() = delete;

        void* mWindow{ nullptr };
    };
}