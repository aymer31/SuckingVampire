#pragma once
#include "core.hpp"
#include "components.hpp"
#include <SFML/Graphics.hpp>

class Enemyrendersystem : public ecs::System
{
public:
    void render(sf::RenderWindow& window)
    {
        for (auto e : entities())
        {
            const auto& pos = ecs::get_component<Position>(e);

            sf::RectangleShape r({50.f, 50.f});
            r.setOrigin(25.f, 25.f);
            r.setPosition(pos.x, pos.y);
            r.setFillColor(sf::Color::Red);

            window.draw(r);
        }
    }
};