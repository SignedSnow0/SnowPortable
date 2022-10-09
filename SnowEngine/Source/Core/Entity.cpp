#include "Entity.h"

namespace Snow {
    b8 Entity::Valid() const { return mScene && mScene->mRegistry.valid(mId); }

    u32 Entity::Id() const { return static_cast<u32>(mId); }

    Scene* Entity::Owner() const { return mScene; }

    Entity::Entity(entt::entity handle, Scene* scene)
        : mId{ handle }, mScene{ scene } { }
}