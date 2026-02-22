#pragma once
#include "core.hpp"
#include "components.hpp"
#include <vector>

class LifetimeSystem : public ecs::System
{
public:
    void update(float dt)
    {
        toDestroy.clear();

        for (auto e : entities())
        {
            auto& life = ecs::get_component<Lifetime>(e);
            life.time -= dt;
            if (life.time <= 0.f)
                toDestroy.push_back(e);
        }

        for (auto e : toDestroy)
            ecs::destroy_entity(e);
    }

private:
    std::vector<ecs::Entity> toDestroy;
};