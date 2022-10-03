#pragma once
#include <filesystem>
#include "Core/Common.h"
#include <imgui.h>

namespace Snow {
    enum class ImageUsage : u32 {
        Image,
        ColorAttachment
    };

    struct ImageCreateInfo {
        ImageUsage Usage;
        std::filesystem::path File;
        u32 Width;
        u32 Height;
    };
    
    class Image {
    public:
        static Image* Create(const ImageCreateInfo& createInfo);
        virtual ~Image() = default;

        virtual ImTextureID GuiId() = 0;
    };
}