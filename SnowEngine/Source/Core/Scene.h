#pragma once
#include <entt/entt.hpp>
#include "Common.h"

namespace Snow {
    class Entity;
    
    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity CreateEntity();
        b8 ExistsEntity(u32 id);
        Entity GetEntity(u32 id);
        b8 TryGetEntity(u32 id, Entity& entity);

        void ExecuteForEachEntity(std::function<void(Entity e)> func);
        
    private:
        entt::registry mRegistry;

        friend class Entity;
    };
}