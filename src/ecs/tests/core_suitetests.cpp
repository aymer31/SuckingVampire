#include "ecs/core.hpp"
#include "ecs/components.hpp"
#include "ecs/meteor_system.hpp"
#include "ecs/exploded_system.hpp"



namespace
{


struct Shaperender final{ 
    std::unique_ptr<sf::Shape> shape;
};


class SpawnSystem : public ecs::System
{
    public:
    void spawn_shape(ecs::Entity entity, std::unique_ptr<sf::Shape> shape)
    {
        auto& shaperender = ecs::get_component<Shaperender>(entity);
        shaperender.shape = std::move(shape);
    }
};


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

class CollisionSystem : public ecs::System
{
    public:
    void update_Collisions()
    {
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
                if (BodySystem::rects_intersect(p1.x, p1.y, b1.width, b1.height, p2.x, p2.y, b2.width, b2.height))
                {
                    // Handle collision between e1 and e2
                
                }
            }
        }
    }
};

class RenderSystem : public ecs::System
{
    public:
    void render_shapes(sf::RenderWindow& window)
    {
        for (const ecs::Entity entity : entities())
        {
            const auto& shaperender = ecs::get_component<Shaperender>(entity);
            if (shaperender.shape)
            {
                window.draw(*shaperender.shape);
            }
        }
    }
};

class BodySystem : public ecs::System
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
/*
TEST(CoreEcsTestSuite, EntityWithDifferentSignatureNotUpdated)
{
    ecs::register_component<Position>();
    ecs::register_component<Motion>();
    ecs::register_component<hitbox>();
    ecs::Signature movementSignature = ecs::create_signature<Position, Motion>();
    auto movementSys = std::make_shared<MovementSystem>();
    ecs::register_system<MovementSystem>(movementSys, movementSignature);
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
    auto bodySys = std::make_shared<BodySystem>();
    ecs::register_system<BodySystem>(bodySys, bodySignature);
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

*/

TEST(CoreEcsTestSuite, ExplodedSystemGeneratesEventsAndDestroysEntities)
{
    ecs::register_component<Position>();
    ecs::register_component<Explosive>();

    ecs::Signature explodedSignature = ecs::create_signature<Position, Explosive>();
    auto explodedSys = std::make_shared<ExplodedSystem>();
    ecs::register_system<ExplodedSystem>(explodedSys, explodedSignature);

    auto e1 = ecs::create_entity();
    auto e2 = ecs::create_entity();

    ecs::add_components(e1, Position{0.f, 0.f}, Explosive{50.f, true});
    ecs::add_components(e2, Position{1.f, 1.f}, Explosive{30.f, false});

    explodedSys->update();

    EXPECT_EQ(explodedSys->events.size(), 1u);
    EXPECT_FLOAT_EQ(explodedSys->events[0].x, 0.f);
    EXPECT_FLOAT_EQ(explodedSys->events[0].y, 0.f);
    EXPECT_FLOAT_EQ(explodedSys->events[0].radius, 50.f);

    auto& pos2 = ecs::get_component<Position>(e2);
    EXPECT_FLOAT_EQ(pos2.x, 1.f);
    EXPECT_FLOAT_EQ(pos2.y, 1.f);

    ecs::destroy_entity(e2);
}


TEST(CoreEcsTestSuite, MeteorSystemMovesEntitiesTowardsTarget)
{
    ecs::register_component<Position>();
    ecs::register_component<Meteor>();
    ecs::register_component<Explosive>();
    ecs::Signature meteorSignature = ecs::create_signature<Position, Meteor, Explosive>();
    auto meteorSys = std::make_shared<MeteorSystem>();
    ecs::register_system<MeteorSystem>(meteorSys, meteorSignature);
    auto e = ecs::create_entity();
    ecs::add_components(e, Position{0.f, 0.f}, Meteor{sf::Vector2f(100.f, 0.f), 10.f}, Explosive{50.f, false});
    meteorSys->update(1.0f);
    auto& pos = ecs::get_component<Position>(e);
    EXPECT_FLOAT_EQ(pos.x, 10.f);
    EXPECT_FLOAT_EQ(pos.y, 0.f);
    meteorSys->update(9.0f);
    EXPECT_FLOAT_EQ(pos.x, 100.f);
    EXPECT_FLOAT_EQ(pos.y, 0.f);
    auto& ex = ecs::get_component<Explosive>(e);
    EXPECT_TRUE(ex.trigger);
    ecs::destroy_entity(e);
}