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
        Entity CreateOrGetEntity(u32 id);
        b8 ExistsEntity(u32 id);
        Entity GetEntity(u32 id);
        b8 TryGetEntity(u32 id, Entity& entity);

        const std::vector<u32>& Entities() const;
        const std::string& Name() const;
        void SetName(const std::string& name);

        void ExecuteForEachEntity(std::function<void(Entity e)> func);
        
    private:
        entt::registry mRegistry;
        std::string mName{ "Test Scene" };
        std::vector<u32> mEntities{};
        
        friend class Entity;
    };
}