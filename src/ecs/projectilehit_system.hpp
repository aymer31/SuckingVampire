#pragma once
#include "core.hpp"
#include "components.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

static inline float dist2f(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return dx * dx + dy * dy;
}

class ProjectileHitSystem : public ecs::System
{
public:
    void update(std::vector<ecs::Entity>& enemies)
    {
        toDestroyProjectiles.clear();

        for (auto p : entities())
        {
            auto* ppos = ecs::try_get_component<Position>(p);
            auto* proj = ecs::try_get_component<Projectile>(p);
            if (!ppos || !proj)
                continue;

            bool hit = false;

            for (auto en : enemies)
            {
                auto* epos = ecs::try_get_component<Position>(en);
                auto* ehp = ecs::try_get_component<Health>(en);
                if (!epos || !ehp)
                    continue;

                float r = proj->radius + 25.f;
                if (dist2f(ppos->x, ppos->y, epos->x, epos->y) <= r * r)
                {
                    ehp->hp -= proj->damage;

                    if (ehp->hp <= 0)
                        ecs::destroy_entity(en);

                    hit = true;
                    break;
                }
            }

            if (hit)
                toDestroyProjectiles.push_back(p);
        }

        for (auto p : toDestroyProjectiles)
            ecs::destroy_entity(p);

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                     [](ecs::Entity e)
                                     {
                                         return !ecs::has_component<Enemy>(e) || !ecs::has_component<Position>(e) ||
                                                !ecs::has_component<Health>(e);
                                     }),
                      enemies.end());
    }

private:
    std::vector<ecs::Entity> toDestroyProjectiles;
};