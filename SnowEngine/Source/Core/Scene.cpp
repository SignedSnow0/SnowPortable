#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Snow {
    Entity Scene::CreateEntity() {
        Entity e{ mRegistry.create(), this };
        e.AddComponent<TransformComponent>();
        return e;
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

    void Scene::ExecuteForEachEntity(std::function<void(Entity e)> func) {
        mRegistry.each([&](auto id) {
            func({ id, this });
        });
    }
}