#include "VkGuiLayer.h"
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "VkCore.h"
#include "Core/Utils.h"

namespace Snow {
    VkGuiLayer::VkGuiLayer(const RenderTarget& rt, RenderPass* scene)
    : mSceneRenderPass{ reinterpret_cast<VkRenderPass*>(scene) } {
        CreateResources(rt.GetSurface());
        InitImGui(rt);

        u32 imageCount{ reinterpret_cast<VkSurface*>(rt.GetSurface())->ImageCount() };
        mTextures.resize(imageCount);
        for (u32 i{ 0 }; i < imageCount; i++) {
            mTextures[i] = scene->Images()[i]->GuiId();
        }
    }

    VkGuiLayer::~VkGuiLayer() {
        vk::Device device{ VkCore::Instance()->Device() };

        device.waitIdle();
        device.destroyDescriptorPool(mDescriptorPool);
        device.destroySampler(mSampler);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        delete mRenderPass;
    }

    void VkGuiLayer::Begin() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGuiID dockspace{ ImGui::DockSpaceOverViewport() };
        if (ImGui::Begin("Style editor")) {
            ImGui::ShowStyleEditor();
        }
        ImGui::End();
        
        ImGui::SetNextWindowDockID(dockspace);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGuiWindowClass windowClass{};
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
        ImGui::SetNextWindowClass(&windowClass);
        if (ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            auto [windowWidth, windowHeight] = ImGui::GetWindowSize();
            b8 resized{ false };
            if ((mSceneRenderPass->Width() != windowWidth || mSceneRenderPass->Height() != windowHeight) && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                mSceneRenderPass->Resize(static_cast<u32>(windowWidth), static_cast<u32>(windowHeight));
                VkSurface::BoundSurface()->AddToDeletionQueue([this](u32 frameIndex) {
                    mTextures[frameIndex] = mSceneRenderPass->Images()[frameIndex]->GuiId();
                });
                resized = true;
            }
           
            if (!resized) {
                    ImGui::Image(mTextures[VkSurface::BoundSurface()->CurrentFrame()], ImGui::GetWindowSize());
            }
            static ImVec2 textSize{ ImGui::CalcTextSize("0000 x 0000") };
            ImGui::SetCursorPos({ windowWidth - textSize.x - 3, 3 });
            ImGui::Text("%u x %u", mSceneRenderPass->Width(), mSceneRenderPass->Height());
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void VkGuiLayer::End() {
        ImGui::Render();

        ImDrawData* drawData = ImGui::GetDrawData();
        
        mRenderPass->Begin();
        ImGui_ImplVulkan_RenderDrawData(drawData, VkSurface::BoundSurface()->CommandBuffer());
        mRenderPass->End();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
    }

    void VkGuiLayer::CreateResources(Surface* surface) {
        vk::DescriptorPoolSize poolSizes[]{
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            { vk::DescriptorType::eInputAttachment, 1000 }
        };

        vk::DescriptorPoolCreateInfo createInfo{};
        createInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        createInfo.maxSets = 1000;
        createInfo.poolSizeCount = static_cast<u32>(std::size(poolSizes));
        createInfo.pPoolSizes = poolSizes;

        mDescriptorPool = VkCore::Instance()->Device().createDescriptorPool(createInfo);

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        mSampler = VkCore::Instance()->Device().createSampler(samplerInfo);

        mRenderPass = new VkRenderPass({ RenderPassUsage::Presentation, surface->Width(), surface->Height(), 2, surface, false });
    }

    void VkGuiLayer::InitImGui(const RenderTarget& rt) {
        ImGui::CreateContext();
        ImGuiIO& io{ ImGui::GetIO() };
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImFont* font{ io.Fonts->AddFontFromFileTTF((Utils::GetFontsPath() / "Inter/Inter-Regular.ttf").string().c_str(), 16.0f) };
        io.Fonts->Build();
        io.FontDefault = font;

        ImGuiPlatformIO& pIo{ ImGui::GetPlatformIO() };
        pIo.Platform_CreateVkSurface = [](ImGuiViewport* vp, ImU64 instance, const void* allocator, ImU64* outSurface) {
            VkSurfaceKHR surface;
            VkResult result = glfwCreateWindowSurface((VkInstance)instance, reinterpret_cast<GLFWwindow*>(vp->PlatformHandle), nullptr, &surface);
            *outSurface = (ImU64)surface;
            
            return static_cast<i32>(result);
        };

        ImGui_ImplGlfw_InitForVulkan(reinterpret_cast<GLFWwindow*>(rt.GetWindow()->Native()), true);

        u32 imageCount = reinterpret_cast<VkSurface*>(rt.GetSurface())->ImageCount();
        
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = VkCore::Instance()->VkInstance();
        initInfo.PhysicalDevice = VkCore::Instance()->PhysicalDevice();
        initInfo.Device = VkCore::Instance()->Device();
        initInfo.QueueFamily = VkCore::Instance()->QueueFamilyIndices().Graphics.first;
        initInfo.Queue = VkCore::Instance()->QueueFamilyIndices().Graphics.second;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = mDescriptorPool;
        initInfo.Allocator = nullptr;
        initInfo.MinImageCount = imageCount;
        initInfo.ImageCount = imageCount;
        initInfo.CheckVkResultFn = nullptr;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        
        ImGui_ImplVulkan_Init(&initInfo, mRenderPass->RenderPass());

        VkCore::Instance()->SubmitInstantCommand([](vk::CommandBuffer cmd) {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}