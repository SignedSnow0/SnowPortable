cmake_minimum_required(VERSION 3.1)

project(SnowEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SnowEngine Source/Core/Application.cpp
                       Source/Core/Logger.cpp
                       
                       Source/Graphics/GraphicsCore.cpp
                       
                       Source/Platform/Vulkan/VkCore.cpp
                       Source/Platform/Window.cpp)

add_subdirectory(External/spdlog)
add_subdirectory(External/glfw)

target_include_directories(SnowEngine PUBLIC Source/ External/spdlog/include/ External/glfw/include/ $ENV{VULKAN_SDK}/include/)

target_link_directories(SnowEngine PUBLIC $ENV{VULKAN_SDK}/lib/)

target_link_libraries(SnowEngine glfw vulkan-1.lib)
