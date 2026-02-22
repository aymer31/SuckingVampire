#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

struct Position
{
    float x{}, y{};
};
struct Motion
{
    float vx{}, vy{};
};

struct hitbox
{
    float width{}, height{};
};

struct Enemy
{
    int hp{3};
};

struct Meteor
{
    sf::Vector2f target;
    float speed{4.f};
    float radius{80.f};

    float fallAcc{0.f};
};
struct MeteorSpawner
{
    float interval{1.0f}; 
    float acc{0.f};       
    float speedMin{150.f};
    float speedMax{260.f};
    float radius{80.f};

    float spawnXMin{0.f}; 
    float spawnXMax{1280.f};
    float spawnY{-50.f}; 

    sf::Vector2f target{640.f, 360.f}; 
};

struct Explosive
{
    float radius{};
    bool trigger{false};
};

struct ExplosionEvent
{
    float x{}, y{}, radius{};
};


struct ShapeComp
{
    std::unique_ptr<sf::Shape> shape;
};
