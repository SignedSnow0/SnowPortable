cmake_minimum_required(VERSION 3.8)

project(SnowEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} SnowEditor/Source/AssetsView.cpp
                               SnowEditor/Source/Controls.cpp
                               SnowEditor/Source/EditorApp.cpp
                               SnowEditor/Source/EntityView.cpp
                               SnowEditor/Source/ResourcesView.cpp
                               SnowEditor/Source/SceneView.cpp
                               SnowEditor/Source/Utils.cpp)

add_subdirectory(SnowEngine/)

target_include_directories(${PROJECT_NAME} PRIVATE SnowEditor/Source/ 
                                                   SnowEngine/Source/
                                                   SnowEngine/External/imgui/)

target_link_libraries(${PROJECT_NAME} SnowEngine)