cmake_minimum_required(VERSION 3.1)

project(SnowEditor)

add_subdirectory(../SnowEngine/ ../SnowEngine/)

add_executable(SnowEditor Source/EditorApp.cpp)

target_include_directories(SnowEditor PUBLIC Source/ ../SnowEngine/Source/)

target_link_libraries(SnowEditor SnowEngine)