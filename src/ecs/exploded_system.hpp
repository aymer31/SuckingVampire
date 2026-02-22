#pragma once
#include "core.hpp"
#include "components.hpp"
#include <vector>

class ExplodedSystem : public ecs::System
{
public:
    std::vector<ExplosionEvent> events;
    std::vector<ecs::Entity> spawnedEnemies;

    void update()
    {
        events.clear();
        spawnedEnemies.clear();
        toDestroy.clear();

        for (auto e : entities())
        {
            auto& ex = ecs::get_component<Explosive>(e);
            if (!ex.trigger)
                continue;

            auto& pos = ecs::get_component<Position>(e);

            // Event explosion
            events.push_back(ExplosionEvent{pos.x, pos.y, ex.radius});

            // Spawn enemy
            ecs::Entity enemy = ecs::create_entity();
            ecs::add_components(enemy, Position{pos.x, pos.y}, Enemy{}, EnemyAI{60.f}, hitbox{50.f, 50.f}, Health{3, 3},
                                DamageOnContact{1, 40.f, 0.5f, 0.f});

            spawnedEnemies.push_back(enemy);
            toDestroy.push_back(e);
        }

        for (auto e : toDestroy)
            ecs::destroy_entity(e);
    }

private:
    std::vector<ecs::Entity> toDestroy;
};