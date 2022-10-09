#pragma once
#include <SnowEngine.h>

class EntityView {
public:
    EntityView() = default;
    ~EntityView() = default;

    void SetEntity(Snow::Entity entity);
    Snow::Entity ActiveEntity() const;
    
    void Draw();

private:
    Snow::Entity mEntity{};
};