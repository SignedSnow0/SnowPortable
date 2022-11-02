#pragma once
#include <filesystem>
#include <SnowEngine.h>

class AssetsView {
public:
    AssetsView(const std::filesystem::path& initialPath);
    ~AssetsView();

    void SetProject(Snow::Project* project);
    
    void Draw();

private:
    std::filesystem::path mCurrentPath;
    Snow::Project* mProject{ nullptr };

    Snow::ResourcePtr<Snow::Image> mFolderIcon;
    Snow::ResourcePtr<Snow::Image> mMeshIcon;
    Snow::ResourcePtr<Snow::Image> mImageIcon;
};