#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Snow {
    Entity Scene::CreateEntity() {
        Entity e{ mRegistry.create(), this };
        e.AddComponent<TransformComponent>();
        auto& comp = e.AddComponent<TagComponent>();
        comp.Tag = "New entity";

        mEntities.emplace_back(e.Id());
        return e;
    }

    Entity Scene::CreateOrGetEntity(u32 id) {
        if (ExistsEntity(id)) {
            return GetEntity(id);
        }
        else {
            return CreateEntity();
        }
    }
    
    Entity Scene::GetEntity(u32 id) {
        if (!ExistsEntity(id)) {
            return { entt::null, nullptr };
        }
        return { entt::entity(id), this };
    }

    b8 Scene::ExistsEntity(u32 id) {
        return mRegistry.valid(entt::entity(id));
    }

    b8 Scene::TryGetEntity(u32 id, Entity& entity) {
        if (ExistsEntity(id)) {
            entity = { entt::entity(id), this };
            return true;
        }
        
        entity = { entt::null, nullptr };
        return false;
    }

    const std::vector<u32>& Scene::Entities() const { return mEntities; }

    const std::string& Scene::Name() const { return mName; }

    void Scene::SetName(const std::string& name) { mName = name; }
    
    void Scene::ExecuteForEachEntity(std::function<void(Entity e)> func) {
        mRegistry.each([&](auto id) {
            func(Entity{ id, this });
        });
    }
}