#pragma once
#include <filesystem>
#include "Common.h"

namespace Snow {
    namespace Utils { 
        std::vector<u8> ReadFile(const std::filesystem::path& file);

        b8 FileDialog(std::filesystem::path& path, const std::filesystem::path& initialPath = std::filesystem::current_path(), const std::vector<std::pair<std::string, std::string>>& filters = {});
        b8 FolderDialog(std::filesystem::path& path, const std::filesystem::path& initialPath = std::filesystem::current_path());

        std::filesystem::path GetCurrentPath();
        std::filesystem::path GetAssetsPath();
        std::filesystem::path GetShadersPath();
        std::filesystem::path GetFontsPath();
        std::filesystem::path GetImagesPath();
    }
}