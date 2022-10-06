#pragma once
#include <filesystem>
#include <SnowEngine.h>

class AssetsView {
public:
    AssetsView(const std::filesystem::path& initialPath);
    ~AssetsView();

    void Draw();

private:
    std::filesystem::path mCurrentPath;

    Snow::Image* mFolderIcon;
};