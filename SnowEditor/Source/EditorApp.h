#include <EntryPoint.h>

class EditorApplication : public Snow::Application {
public:
    EditorApplication();
    virtual ~EditorApplication() override;
};

void CreateApplication(Snow::Application*& app) {
    app = new EditorApplication();
}

void DestroyApplication(Snow::Application* app) {
    delete app;
}