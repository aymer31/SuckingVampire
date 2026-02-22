#pragma once

#include "core.hpp"
#include "components.hpp"
#include <vector>

class MovementSystem : public ecs::System
{
public:
    void update_Positions(float dt)
    {
        for (const ecs::Entity entity : entities())
        {
            auto& pos = ecs::get_component<Position>(entity);
            auto& vel = ecs::get_component<Motion>(entity);
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
};
