cmake_minimum_required(VERSION 3.8)

project(SnowEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(../SnowEngine/ ../SnowEngine/)

add_executable(SnowEditor Source/EditorApp.cpp)

target_include_directories(SnowEditor PUBLIC Source/ ../SnowEngine/Source/)

target_link_libraries(SnowEditor SnowEngine)

