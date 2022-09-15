#include "Window.h"
#include <GLFW/glfw3.h>
#include "Core/Logger.h"

namespace Snow {
    static b8 GlfwInitialized{ false };
    
    Window::Window(const std::string& title, u32 width, u32 height, b8 visible) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
        
        mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!mWindow) {
            LOG_ERROR("Failed to create window");
        }
    }

    Window::~Window() {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(mWindow));
    }

    decltype(auto) Window::Native() { return reinterpret_cast<GLFWwindow*>(mWindow); }

    b8 Window::ShouldClose() { return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(mWindow)); }

    b8 Window::Initialize() {
        if (GlfwInitialized) {
            LOG_WARN("Window system already initialized");
            return false;
        }
        
        glfwInit();
        GlfwInitialized = true;

        LOG_INFO("Window system initialized");
        return true;
    }

    void Window::Shutdown() {
        if (!GlfwInitialized) {
            return;
        }
        
        glfwTerminate();
        GlfwInitialized = false;
        LOG_INFO("Window system shutdown");
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }
}