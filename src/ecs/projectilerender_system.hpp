#pragma once
#include "core.hpp"
#include "components.hpp"
#include <SFML/Graphics.hpp>

class ProjectileRenderSystem : public ecs::System
{
public:
    void render(sf::RenderWindow& window)
    {
        for (auto e : entities())
        {
            const auto& pos = ecs::get_component<Position>(e);

            sf::CircleShape c(6.f);
            c.setOrigin(6.f, 6.f);
            c.setFillColor(sf::Color::Cyan);
            c.setPosition(pos.x, pos.y);

            window.draw(c);
        }
    }
};