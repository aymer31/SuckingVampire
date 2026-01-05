#include <gtest/gtest.h>

#include "ecs/core.hpp"
#include "ecs/internal/system_manager.hpp"

#include <vector>
#include <algorithm>
#include <unordered_map>

namespace
{
struct Position final
{
    float x{}, y{};
};

struct Motion final
{
    float vx{}, vy{};
};

struct hitbox final
{
    float width{}, height{};
};

class movementSystem : public ecs::System
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

class bodySystem : public ecs::System
{
public:
    static bool rects_intersect(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh)
    {
        return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
    }

    void update_Hitboxes()
    {
        m_overlaps.clear();

        for (auto it1 = entities().begin(); it1 != entities().end(); ++it1)
        {
            auto e1 = *it1;
            const auto& p1 = ecs::get_component<Position>(e1);
            const auto& b1 = ecs::get_component<hitbox>(e1);

            auto it2 = it1;
            ++it2;
            for (; it2 != entities().end(); ++it2)
            {
                auto e2 = *it2;
                const auto& p2 = ecs::get_component<Position>(e2);
                const auto& b2 = ecs::get_component<hitbox>(e2);

                if (rects_intersect(p1.x, p1.y, b1.width, b1.height, p2.x, p2.y, b2.width, b2.height))
                {
                    m_overlaps[e1].push_back(e2);
                    m_overlaps[e2].push_back(e1);
                }
            }
        }
    }
    std::vector<ecs::Entity> get_overlaps(ecs::Entity entity) const
    {
        auto it = m_overlaps.find(entity);
        if (it == m_overlaps.end())
            return {};
        return it->second;
    }

    std::size_t entity_count()
    {
        return entities().size();
    }

private:
    std::unordered_map<ecs::Entity, std::vector<ecs::Entity>> m_overlaps{};
};

} // namespace

TEST(CoreEcsTestSuite, EntityWithDifferentSignatureNotUpdated)
{
    ecs::register_component<Position>();
    ecs::register_component<Motion>();
    ecs::register_component<hitbox>();
    ecs::Signature movementSignature = ecs::create_signature<Position, Motion>();
    auto movementSys = std::make_shared<movementSystem>();
    ecs::register_system<movementSystem>(movementSys, movementSignature);
    const ecs::Entity e = ecs::create_entity();
    ecs::add_components(e, Position{0.f, 0.f}, hitbox{2.f, 2.f});
    movementSys->update_Positions(1.0f);
    auto& pos = ecs::get_component<Position>(e);
    EXPECT_FLOAT_EQ(pos.x, 0.f);
    EXPECT_FLOAT_EQ(pos.y, 0.f);
    ecs::destroy_entity(e);
}

TEST(CoreEcsTestSuite, BodySystemDetectsMultipleCollisions)
{
    ecs::register_component<Position>();
    ecs::register_component<hitbox>();
    ecs::Signature bodySignature = ecs::create_signature<Position, hitbox>();
    auto bodySys = std::make_shared<bodySystem>();
    ecs::register_system<bodySystem>(bodySys, bodySignature);
    const ecs::Entity a = ecs::create_entity();
    const ecs::Entity b = ecs::create_entity();
    const ecs::Entity c = ecs::create_entity();
    ecs::add_components(a, Position{0.f, 0.f}, hitbox{10.f, 10.f});     
    ecs::add_components(b, Position{5.f, 5.f}, hitbox{10.f, 10.f});     
    ecs::add_components(c, Position{100.f, 100.f}, hitbox{10.f, 10.f});
    EXPECT_EQ(bodySys->entity_count(), 3u);
    bodySys->update_Hitboxes();
    auto overlaps = bodySys->get_overlaps(a);
    EXPECT_EQ(overlaps.size(), 1u);
    EXPECT_EQ(overlaps[0], b);
    auto overlapsB = bodySys->get_overlaps(b);
    EXPECT_EQ(overlapsB.size(), 1u);
    EXPECT_EQ(overlapsB[0], a);
    auto overlapsC = bodySys->get_overlaps(c);
    EXPECT_TRUE(overlapsC.empty());

    ecs::destroy_entity(a);
    ecs::destroy_entity(b);
    ecs::destroy_entity(c);
}