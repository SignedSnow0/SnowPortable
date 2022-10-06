#include "Image.h"
#include "Graphics/GraphicsCore.h"
#include "Core/Logger.h"
#include "Platform/Vulkan/VkImage.h"

namespace Snow {
    Image* Image::Create(const ImageCreateInfo& createInfo) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkImage(createInfo);
        }

        LOG_ERROR("Tried to create image with invalid Graphics API!");
        return nullptr;
    }
}