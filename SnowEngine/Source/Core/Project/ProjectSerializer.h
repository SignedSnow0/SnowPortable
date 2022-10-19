#pragma once 
#include "Project.h"

namespace Snow {
    class ProjectSerializer {
    public:
        static void Serialize(Project& project);
        static Project* Deserialize(const std::filesystem::path& path);

    private:
        
    };
}