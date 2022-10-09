project(SnowEngine)

#set properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#add source files
add_library(${PROJECT_NAME} Source/Core/Application.cpp
                            Source/Core/Components.cpp
                            Source/Core/Entity.cpp
                            Source/Core/Input.cpp
                            Source/Core/Logger.cpp
                            Source/Core/Scene.cpp
                            Source/Core/Utils.cpp
                       
                            Source/Graphics/Rhi/Buffers.cpp
                            Source/Graphics/Rhi/DescriptorSet.cpp
                            Source/Graphics/Rhi/GuiLayer.cpp
                            Source/Graphics/Rhi/Image.cpp
                            Source/Graphics/Rhi/Pipeline.cpp
                            Source/Graphics/Rhi/RenderPass.cpp
                            Source/Graphics/Rhi/Shader.cpp
                            Source/Graphics/Rhi/Surface.cpp
                            Source/Graphics/Camera.cpp
                            Source/Graphics/GraphicsCore.cpp
                            Source/Graphics/RenderTarget.cpp
                            Source/Graphics/SceneRenderer.cpp

                            Source/Platform/Vulkan/VkBuffers.cpp
                            Source/Platform/Vulkan/VkDescriptorSet.cpp
                            Source/Platform/Vulkan/VkGuiLayer.cpp
                            Source/Platform/Vulkan/VkImage.cpp
                            Source/Platform/Vulkan/VkCore.cpp
                            Source/Platform/Vulkan/VkPipeline.cpp
                            Source/Platform/Vulkan/VkRenderPass.cpp
                            Source/Platform/Vulkan/VkShader.cpp
                            Source/Platform/Vulkan/VkSurface.cpp
                            Source/Platform/Window.cpp
                            
                            External/imgui/imgui.cpp
                            External/imgui/imgui_demo.cpp
                            External/imgui/imgui_tables.cpp
                            External/imgui/imgui_draw.cpp
                            External/imgui/imgui_widgets.cpp
                            External/imgui/backends/imgui_impl_glfw.cpp
                            External/imgui/backends/imgui_impl_vulkan.cpp)

#add libraries
add_subdirectory(External/spdlog)
add_subdirectory(External/glfw)
add_subdirectory(External/glm)
add_subdirectory(External/vma)

#add include folders
target_include_directories(${PROJECT_NAME} PUBLIC Source/ 
                                                  External/spdlog/include/ 
                                                  External/glfw/include/ 
                                                  External/glm/ 
                                                  External/vma/include/ 
                                                  External/imgui/ 
                                                  External/stb/ 
                                                  External/entt/single_include/ 
                                                  $ENV{VULKAN_SDK}/include/)

#link libraries
target_link_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/lib/)
target_link_libraries(${PROJECT_NAME} glfw 
                                      vulkan-1.lib)