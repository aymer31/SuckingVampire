#include "Game/game.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Common/common.h"
#include "ecs/core.hpp"
#include "ecs/components.hpp"
#include "ecs/meteor_system.hpp"
#include "ecs/exploded_system.hpp"
#include "ecs/damage_system.hpp"
#include "ecs/enemychase_system.hpp"
#include "ecs/playerautoattack_system.hpp"
#include "ecs/projectilerender_system.hpp"
#include "ecs/movement_system.hpp"
#include "ecs/lifetime_system.hpp"
#include "ecs/projectilehit_system.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

static float randf(float a, float b)
{
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> dist(std::min(a, b), std::max(a, b));
    return dist(rng);
}

static float distance(sf::Vector2f a, sf::Vector2f b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

static void player_input(ecs::Entity player, float dt)
{
    auto& pos = ecs::get_component<Position>(player);
    const float speed = 350.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        pos.y -= speed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        pos.y += speed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        pos.x -= speed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        pos.x += speed * dt;
}

static void purge_dead_meteors(std::vector<ecs::Entity>& meteors)
{
    meteors.erase(std::remove_if(meteors.begin(), meteors.end(), [](ecs::Entity e)
                                 { return !ecs::has_component<Position>(e) || !ecs::has_component<Meteor>(e); }),
                  meteors.end());
}

static void purge_dead_enemies(std::vector<ecs::Entity>& enemies)
{
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](ecs::Entity e)
                                 { return !ecs::has_component<Position>(e) || !ecs::has_component<Enemy>(e); }),
                  enemies.end());
}

struct GameRuntime
{
    ecs::Entity player{};

    std::shared_ptr<MeteorSystem> meteorSys;
    std::shared_ptr<ExplodedSystem> explodedSys;
    std::shared_ptr<EnemyChaseSystem> enemyChaseSys;
    std::shared_ptr<DamageSystem> damageSys;
    std::shared_ptr<PlayerAutoAttackSystem> playerAttackSys;
    std::shared_ptr<ProjectileRenderSystem> projectileRenderSys;
    std::shared_ptr<MovementSystem> moveSys;
    std::shared_ptr<LifetimeSystem> lifeSys;
    std::shared_ptr<ProjectileHitSystem> projectileHitSys;

    std::vector<ecs::Entity> meteors;
    std::vector<ecs::Entity> enemies;

    float meteorDelay{1.0f};
    float meteorAcc{0.f};
    int maxMeteors{10};
    float spawnRange{600.f};
    float spawnAbove{300.f};

    float meteorSpeed{400.f};
    float meteorRadius{120.f};

    int explosionDamage{2};
    float explosionIFrames{0.8f};
};

static bool build_game(GameRuntime& rt)
{
    ecs::register_component<Position>();
    ecs::register_component<hitbox>();
    ecs::register_component<Enemy>();
    ecs::register_component<Meteor>();
    ecs::register_component<Explosive>();
    ecs::register_component<ExplosionEvent>();
    ecs::register_component<EnemyAI>();
    ecs::register_component<Health>();
    ecs::register_component<DamageOnContact>();
    ecs::register_component<Invincibility>();
    ecs::register_component<Velocity>();
    ecs::register_component<Projectile>();
    ecs::register_component<Lifetime>();
    ecs::register_component<AutoAttack>();
    ecs::register_component<Motion>();
    ecs::register_component<Lifetime>();


    rt.meteorSys = std::make_shared<MeteorSystem>();
    ecs::register_system<MeteorSystem>(rt.meteorSys, ecs::create_signature<Position, Meteor, Explosive>());

    rt.explodedSys = std::make_shared<ExplodedSystem>();
    ecs::register_system<ExplodedSystem>(rt.explodedSys, ecs::create_signature<Position, Explosive>());

    rt.enemyChaseSys = std::make_shared<EnemyChaseSystem>();
    ecs::register_system<EnemyChaseSystem>(rt.enemyChaseSys, ecs::create_signature<Position, Enemy>());

    rt.damageSys = std::make_shared<DamageSystem>();
    ecs::register_system<DamageSystem>(rt.damageSys, ecs::create_signature<Position, Enemy, DamageOnContact>());

    rt.playerAttackSys = std::make_shared<PlayerAutoAttackSystem>();
    ecs::register_system<PlayerAutoAttackSystem>(rt.playerAttackSys, ecs::create_signature<Position, AutoAttack>());

    rt.projectileRenderSys = std::make_shared<ProjectileRenderSystem>();
    ecs::register_system<ProjectileRenderSystem>(rt.projectileRenderSys, ecs::create_signature<Position, Projectile>());

    rt.moveSys = std::make_shared<MovementSystem>();
    ecs::register_system<MovementSystem>(rt.moveSys, ecs::create_signature<Position, Motion>());

    rt.lifeSys = std::make_shared<LifetimeSystem>();
    ecs::register_system<LifetimeSystem>(rt.lifeSys, ecs::create_signature<Lifetime>());

    rt.projectileHitSys = std::make_shared<ProjectileHitSystem>();
    ecs::register_system<ProjectileHitSystem>(rt.projectileHitSys, ecs::create_signature<Position, Projectile>());

    rt.player = ecs::create_entity();
    ecs::add_components(rt.player, Position{2500.f, 2500.f}, hitbox{50.f, 50.f}, Health{10, 10},
                        AutoAttack{1.f, 0.f, 8, 700.f, 1.2f, 1, 25.f});



    rt.meteors.reserve(64);
    rt.enemies.reserve(64);

    return true;
}

void run_game()
{
    std::cout << "[Game] entered run_game()\n";

    sf::RenderWindow game(sf::VideoMode(widthWindow, heightWindow), "Game");
    game.setFramerateLimit(120);

    sf::View camera(sf::FloatRect(0.f, 0.f, (float)widthWindow, (float)heightWindow));

    sf::Texture bgTex;
    if (!bgTex.loadFromFile("assets/background.png"))
    {
        std::cerr << "[Game] Missing asset: assets/background.png\n";
        return;
    }
    bgTex.setRepeated(true);
    sf::Sprite background(bgTex);
    background.setTextureRect(sf::IntRect(0, 0, 5000, 5000));

    GameRuntime rt;
    if (!build_game(rt))
        return;

    sf::Clock clock;

    while (game.isOpen())
    {
        sf::Event e;
        while (game.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                game.close();
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                game.close();
        }

        float dt = clock.restart().asSeconds();

        player_input(rt.player, dt);

        const auto& ppos = ecs::get_component<Position>(rt.player);
        camera.setCenter(ppos.x, ppos.y);
        game.setView(camera);

        purge_dead_meteors(rt.meteors);
        purge_dead_enemies(rt.enemies);

        rt.meteorAcc += dt;
        if (rt.meteorAcc >= rt.meteorDelay && (int)rt.meteors.size() < rt.maxMeteors)
        {
            rt.meteorAcc = 0.f;

            float x = ppos.x + randf(-rt.spawnRange * 0.5f, rt.spawnRange * 0.5f);
            float y = ppos.y + randf(-rt.spawnRange * 0.5f, rt.spawnRange * 0.5f);

            float SOL_Y = 3000.f;

            ecs::Entity m = ecs::create_entity();
            ecs::add_components(m, Position{x, y - rt.spawnAbove},
                                Meteor{sf::Vector2f(x, SOL_Y), rt.meteorSpeed, rt.meteorRadius, 0.f},
                                Explosive{rt.meteorRadius, false});

            rt.meteors.push_back(m);
        }

        rt.meteorSys->update(dt);
        rt.explodedSys->update();
        rt.enemyChaseSys->update(dt, rt.player);
        rt.damageSys->update(dt, rt.player);
        rt.playerAttackSys->update(dt);
        rt.playerAttackSys->update(dt);
        rt.moveSys->update_Positions(dt);
        rt.lifeSys->update(dt);
        rt.projectileHitSys->update(rt.enemies);

        for (auto en : rt.explodedSys->spawnedEnemies)
            rt.enemies.push_back(en);
        
        {
            auto& php = ecs::get_component<Health>(rt.player);

            float invTime = 0.f;
            if (auto* inv = ecs::try_get_component<Invincibility>(rt.player))
                invTime = inv->time;

            for (const auto& ev : rt.explodedSys->events)
            {
                if (distance(sf::Vector2f(ppos.x, ppos.y), sf::Vector2f(ev.x, ev.y)) <= ev.radius)
                {
                    if (invTime <= 0.f)
                    {
                        php.hp -= rt.explosionDamage;

                        if (!ecs::has_component<Invincibility>(rt.player))
                            ecs::add_components(rt.player, Invincibility{rt.explosionIFrames});
                        else
                            ecs::get_component<Invincibility>(rt.player).time = rt.explosionIFrames;
                    }
                    break; 
                }
            }
        }

        {
            const auto& hp = ecs::get_component<Health>(rt.player);
            if (hp.hp <= 0)
            {
                std::cout << "[Game] GAME OVER\n";
                game.close();
            }
        }

        game.clear();
        game.draw(background);
        rt.projectileRenderSys->render(game);

        for (auto en : rt.enemies)
        {
            auto* pos = ecs::try_get_component<Position>(en);
            if (!pos)
                continue;

            sf::RectangleShape shape(sf::Vector2f(50.f, 50.f));
            shape.setOrigin(25.f, 25.f);
            shape.setFillColor(sf::Color::Red);
            shape.setPosition(pos->x, pos->y);
            game.draw(shape);
        }

        for (auto m : rt.meteors)
        {
            auto* pos = ecs::try_get_component<Position>(m);
            if (!pos)
                continue;

            sf::CircleShape shape(15.f);
            shape.setOrigin(15.f, 15.f);
            shape.setFillColor(sf::Color::Yellow);
            shape.setPosition(pos->x, pos->y);
            game.draw(shape);
        }

        {
            sf::RectangleShape rect(sf::Vector2f(50.f, 50.f));
            rect.setOrigin(25.f, 25.f);
            rect.setFillColor(sf::Color::Green);
            rect.setPosition(ppos.x, ppos.y);
            game.draw(rect);
        }

        game.display();
    }
}