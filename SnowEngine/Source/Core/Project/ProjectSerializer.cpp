#include "ProjectSerializer.h"
#include <fstream>
#include "Core/Scene.h"
#include "Core/Entity.h"
#include "Core/Components.h"

namespace Snow {
  
    void ProjectSerializer::Serialize(const Project& project) {
        if (!std::filesystem::exists(project.Root())) {
            std::filesystem::create_directory(project.Root());
        }
    }

    Project ProjectSerializer::Deserialize(const std::filesystem::path& path) {
        return { "", "" };
    }
}