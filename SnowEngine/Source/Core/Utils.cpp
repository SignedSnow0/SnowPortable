#include "Utils.h"
#include <fstream>
#include "Core/Logger.h"

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

		std::filesystem::path GetCurrentPath() { return std::filesystem::current_path() / "SnowEngine"; }

		std::filesystem::path GetAssetsPath() { return GetCurrentPath() / "Assets"; }

		std::filesystem::path GetShadersPath() { return GetAssetsPath() / "Shaders"; }
	}
}