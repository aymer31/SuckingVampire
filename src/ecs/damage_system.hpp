#pragma once
#include "core.hpp"
#include "components.hpp"
#include <cmath>
#include <algorithm>

static inline float dist2(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return dx * dx + dy * dy;
}

class DamageSystem : public ecs::System
{
public:
    // Ennemi -> Joueur au contact
    void update(float dt, ecs::Entity player)
    {
        if (!ecs::has_component<Position>(player) || !ecs::has_component<Health>(player))
            return;

        auto& ppos = ecs::get_component<Position>(player);
        auto& php = ecs::get_component<Health>(player);

        // tick invincibility player
        if (auto* inv = ecs::try_get_component<Invincibility>(player))
            inv->time = std::max(0.f, inv->time - dt);

        for (auto e : entities()) // doit contenir Enemy + Position + DamageOnContact
        {
            auto& epos = ecs::get_component<Position>(e);
            auto& doc = ecs::get_component<DamageOnContact>(e);

            doc.acc = std::max(0.f, doc.acc - dt);

            float r = doc.radius;
            if (dist2(epos.x, epos.y, ppos.x, ppos.y) <= r * r)
            {
                float invTime = 0.f;
                if (auto* invP = ecs::try_get_component<Invincibility>(player))
                    invTime = invP->time;

                if (invTime <= 0.f && doc.acc <= 0.f)
                {
                    php.hp -= doc.damage;
                    doc.acc = doc.cooldown;

                    // i-frames joueur
                    if (!ecs::has_component<Invincibility>(player))
                        ecs::add_components(player, Invincibility{0.6f});
                    else
                        ecs::get_component<Invincibility>(player).time = 0.6f;
                }
            }
        }
    }
};