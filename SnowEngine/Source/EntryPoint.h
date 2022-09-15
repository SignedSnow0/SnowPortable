#pragma once
#include "SnowEngine.h"

extern void CreateApplication(Snow::Application*& app);
extern void DestroyApplication(Snow::Application* app);

int main(int argc, char** argv) {
    Snow::Application* app{ nullptr };
    CreateApplication(app);
    
    if (!app) {
        return 1;
    }
    
    app->Run();
    DestroyApplication(app);

    return 0;
}
