#include "Utils.h"
#include <fstream>
#include "Core/Logger.h"
#include <nfd.hpp>

namespace Snow {
    namespace Utils{
		std::vector<u8> ReadFile(const std::filesystem::path& file) {
			std::ifstream stream{ file, std::ios::ate | std::ios::binary };
			if (!stream.is_open()) {
            	LOG_ERROR("Failed to read file at: {}", file.string());
				return {};
			}

			u64 size{ static_cast<u64>(stream.tellg()) };
			std::vector<u8> buffer{};
			buffer.resize(size);

			stream.seekg(0);
			stream.read((char*)buffer.data(), size);

			stream.close();
			return buffer;
		}

		b8 FileDialog(std::filesystem::path& path, const std::filesystem::path& initialPath, const std::vector<std::pair<std::string, std::string>>& filters) {
			char* outPath;
			std::vector<nfdfilteritem_t> filterItems{ filters.size() };
			for (u32 i = 0; i < filters.size(); i++) {
				filterItems[i].name = filters[i].first.c_str();
				filterItems[i].spec = filters[i].second.c_str();
			}

			if (NFD_OpenDialog(&outPath, filterItems.data(), filterItems.size(), initialPath.string().c_str()) == NFD_OKAY) {
				path = outPath;
				NFD_FreePath(outPath);
				return true;
			}
			
			return false;
		}

		b8 FolderDialog(std::filesystem::path& path, const std::filesystem::path& initialPath) {
			char* outPath;
			if (NFD_PickFolder(&outPath, initialPath.string().c_str()) == NFD_OKAY) {
				path = outPath;
				NFD_FreePath(outPath);
				return true;
			}

			return false;
		}

		std::filesystem::path GetCurrentPath() { return std::filesystem::current_path() / "SnowEngine"; }

		std::filesystem::path GetAssetsPath() { return GetCurrentPath() / "Assets"; }

		std::filesystem::path GetShadersPath() { return GetAssetsPath() / "Shaders"; }

		std::filesystem::path GetFontsPath() { return GetAssetsPath() / "Fonts"; }

		std::filesystem::path GetImagesPath() { return GetAssetsPath() / "Images"; }
	}
}