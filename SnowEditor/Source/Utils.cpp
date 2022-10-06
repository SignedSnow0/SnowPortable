#include "Utils.h"

namespace Utils{
	std::filesystem::path GetCurrentPath() { return std::filesystem::current_path() / "SnowEditor"; }

	std::filesystem::path GetAssetsPath() { return GetCurrentPath() / "Assets"; }

	std::filesystem::path GetImagesPath() { return GetAssetsPath() / "Images"; }
}