#pragma once
#include <filesystem>

namespace Utils { 
    std::filesystem::path GetCurrentPath();
    std::filesystem::path GetAssetsPath();
    std::filesystem::path GetImagesPath();
}