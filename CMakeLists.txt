cmake_minimum_required(VERSION 3.8)

project(SnowEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} SnowEditor/Source/EditorApp.cpp)

add_subdirectory(SnowEngine/)

target_include_directories(${PROJECT_NAME} PRIVATE SnowEditor/Source/ 
                                                   SnowEngine/Source/)

target_link_libraries(${PROJECT_NAME} SnowEngine)