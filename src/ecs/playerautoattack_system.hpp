#pragma once
#include "core.hpp"
#include "components.hpp"
#include <cmath>

struct AutoAttack
{
    float interval{1.f};
    float acc{0.f};
    int bullets{8};
    float speed{700.f};
    float lifetime{1.2f};
    int damage{1};
    float spawnRadius{25.f};
};

class PlayerAutoAttackSystem : public ecs::System
{
public:
    void update(float dt)
    {
        for (auto e : entities())
        {
            auto& pos = ecs::get_component<Position>(e);
            auto& atk = ecs::get_component<AutoAttack>(e);

            atk.acc += dt;
            if (atk.acc < atk.interval)
                continue;

            atk.acc = 0.f;

            const float twoPi = 6.28318530718f;

            for (int i = 0; i < atk.bullets; ++i)
            {
                float angle = (twoPi * i) / (float)atk.bullets;
                float dx = std::cos(angle);
                float dy = std::sin(angle);

                ecs::Entity p = ecs::create_entity();

                ecs::add_components(p, Position{pos.x + dx * atk.spawnRadius, pos.y + dy * atk.spawnRadius},
                                    Motion{dx * atk.speed, dy * atk.speed}, Projectile{atk.damage, 10.f},
                                    Lifetime{atk.lifetime});
            }
        }
    }
};