#pragma once
#include "ecs/core.hpp"
#include "components.hpp"
#include <random>
#include <algorithm>

class EntitySpawnSystem : public ecs::System
{
public:
    void update(float dt)
    {
        for (auto e : entities())
        {
            auto& sp = ecs::get_component<MeteorSpawner>(e);

            sp.acc += dt;
            while (sp.acc >= sp.interval)
            {
                sp.acc -= sp.interval;
                spawn_meteor(sp);
            }
        }
    }

private:
    void spawn_meteor(const MeteorSpawner& sp)
    {
        ecs::Entity m = ecs::create_entity();

        const float x = randf(sp.spawnXMin, sp.spawnXMax);
        const float speed = randf(sp.speedMin, sp.speedMax);

        ecs::add_components(m, Position{x, sp.spawnY}, Meteor{sp.target, speed, sp.radius},
                            Explosive{sp.radius, false});
    }

    static float randf(float a, float b)
    {
        static thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<float> dist(std::min(a, b), std::max(a, b));
        return dist(rng);
    }
};