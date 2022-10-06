#pragma once
#include <entt/entt.hpp>
#include "Scene.h"
#include "Common.h"

namespace Snow {
    class Entity {
    public:
        Entity() = default;
        ~Entity() = default;

        template<typename Comp, typename... Args>
        Comp& AddComponent(Args&&... args) {
            if (HasComponent<Comp>()) {
                return GetComponent<Comp>();
            }
            
            return mScene->mRegistry.emplace<Comp>(mId, std::forward<Args>(args)...);
        }

        template<typename Comp>
        void RemoveComponent() {
            mScene->mRegistry.remove<Comp>(mId);
        }

        template<typename... Comp>
        b8 HasComponent() const {
            return mScene->mRegistry.all_of<Comp ...>(mId);
        }

        template<typename Comp>
        Comp& GetComponent() {
            return mScene->mRegistry.get<Comp>(mId);
        }

        template<typename... Comp>
        auto TryGetComponent() {
            return mScene->mRegistry.try_get<Comp ...>(mId);
        }

        b8 Valid() const;
        u32 Id() const;
        Scene* Owner() const;
        
    private:
        Entity(entt::entity handle, Scene* scene);
        
        entt::entity mId{ entt::null };
        Scene* mScene{ nullptr };

        friend class Scene;
    };
}