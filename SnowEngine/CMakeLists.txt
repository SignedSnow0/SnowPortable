project(SnowEngine)

add_library(SnowEngine Source/Core/Application.cpp
                       Source/Core/Logger.cpp
                       Source/Platform/Window.cpp)

add_subdirectory(External/glfw)

target_include_directories(SnowEngine PUBLIC Source/ External/spdlog/include/ External/glfw/include/)

target_link_libraries(SnowEngine glfw)
