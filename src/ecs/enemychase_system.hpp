#pragma once
#include "core.hpp"
#include "components.hpp"
#include <cmath>

class EnemyChaseSystem : public ecs::System
{
public:
    void update(float dt, ecs::Entity player)
    {
        if (!ecs::has_component<Position>(player))
            return;

        const auto& ppos = ecs::get_component<Position>(player);

        for (auto e : entities())
        {
            auto& pos = ecs::get_component<Position>(e);

            float speed = 160.f;
            if (ecs::has_component<EnemyAI>(e))
                speed = ecs::get_component<EnemyAI>(e).speed;

            float dx = ppos.x - pos.x;
            float dy = ppos.y - pos.y;
            float len = std::sqrt(dx * dx + dy * dy);

            if (len < 0.001f)
                continue;

            dx /= len;
            dy /= len;

            pos.x += dx * speed * dt;
            pos.y += dy * speed * dt;
        }
    }
};