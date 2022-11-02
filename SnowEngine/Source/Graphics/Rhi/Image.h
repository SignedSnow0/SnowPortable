#pragma once
#include <filesystem>
#include "Core/Common.h"
#include <imgui.h>
#include "Core/ResourcePtr.h"

namespace Snow {
    enum class ImageUsage : u32 {
        Image,
        ColorAttachment,
        DepthAttachment
    };

    struct ImageCreateInfo {
        ImageUsage Usage;
        std::filesystem::path File;
        u8* Data{ nullptr };
        u32 Width{ 0 };
        u32 Height{ 0 };
    };
    
    class Image : public Resource<Image> {
    public:
        static Image* Create(const ImageCreateInfo& createInfo);
        virtual ~Image() = default;

        virtual ImTextureID GuiId() = 0;
        virtual std::filesystem::path Path() const = 0;
    };
}