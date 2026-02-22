#pragma once
#include "core.hpp"
#include "components.hpp"

class MeteorSystem : public ecs::System
{
public:
    float gridStep = 32.f;
    float speedScale = 2.f; 

    void update(float dt)
    {
        for (auto e : entities())
        {
            auto& pos = ecs::get_component<Position>(e);
            auto& met = ecs::get_component<Meteor>(e);
            auto& ex = ecs::get_component<Explosive>(e);

            if (ex.trigger)
                continue;

            met.fallAcc += met.speed * dt * speedScale;

            while (met.fallAcc >= gridStep)
            {
                pos.y += gridStep;
                met.fallAcc -= gridStep;

                if (pos.y >= met.target.y)
                {
                    pos.y = met.target.y;
                    ex.trigger = true;
                    met.fallAcc = 0.f;
                    break;
                }
            }
        }
    }
};