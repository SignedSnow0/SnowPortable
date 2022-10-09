#include "Window.h"
#include <GLFW/glfw3.h>
#include "Core/Logger.h"
#include "Core/Input.h"

namespace Snow {
    static b8 GlfwInitialized{ false };
    
    Window::Window(const std::string& title, u32 width, u32 height, b8 visible) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);

        mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!mWindow) {
            LOG_ERROR("Failed to create window");
        }

        //TODO: Differentiate between gui and no gui layer
        glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(mWindow), [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
            Input::SetKey(static_cast<Key>(key), action != GLFW_RELEASE);
        });

        glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(mWindow), [](GLFWwindow* window, i32 button, i32 action, i32 mods) {
            Input::SetButton(static_cast<Button>(button), action != GLFW_RELEASE);
        });

        glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(mWindow), [](GLFWwindow* window, f64 xpos, f64 ypos) {
            Input::SetMousePosition(vec2i{ static_cast<f32>(xpos), static_cast<f32>(ypos) });
        });

        glfwSetScrollCallback(reinterpret_cast<GLFWwindow*>(mWindow), [](GLFWwindow* window, f64 xoffset, f64 yoffset) {
            Input::SetWheelDelta(static_cast<f32>(yoffset));
        });
    }

    Window::~Window() {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(mWindow));
    }

    void* Window::Native() const { return mWindow; }

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