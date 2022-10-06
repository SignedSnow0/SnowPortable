#pragma once
#include <filesystem>
#include "Common.h"

namespace Snow {
    namespace Utils { 
        std::vector<u8> ReadFile(const std::filesystem::path& file);

        std::filesystem::path GetCurrentPath();
        std::filesystem::path GetAssetsPath();
        std::filesystem::path GetShadersPath();
        std::filesystem::path GetFontsPath();
        std::filesystem::path GetImagesPath();
    }
}