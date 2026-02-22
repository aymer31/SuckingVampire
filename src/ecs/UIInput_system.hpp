#pragma once
#include "ecs/core.hpp"
#include "components.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>

class UIInputSystem : public ecs::System
{
public:
    std::optional<int> firedAction;

    void update(sf::RenderWindow& window)
    {
        firedAction.reset();

        const sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));

        for (auto e : entities())
        {
            auto& tr = ecs::get_component<UITransform>(e);
            auto& btn = ecs::get_component<UIButton>(e);

            bool hover = false;

            if (ecs::has_component<UIRect>(e))
            {
                const auto& rc = ecs::get_component<UIRect>(e);
                hover =
                    (mouse.x >= tr.x && mouse.x <= tr.x + rc.width && mouse.y >= tr.y && mouse.y <= tr.y + rc.height);
            }
            else if (ecs::has_component<UICircle>(e))
            {
                const auto& cc = ecs::get_component<UICircle>(e);
                const float cx = tr.x + cc.radius;
                const float cy = tr.y + cc.radius;
                const float dx = mouse.x - cx;
                const float dy = mouse.y - cy;
                hover = (dx * dx + dy * dy) <= (cc.radius * cc.radius);
            }

            btn.hovered = hover;
            btn.selected = hover;
        }

        static bool wasDown = false;
        const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        if (down && !wasDown)
        {
            for (auto e : entities())
            {
                const auto& btn = ecs::get_component<UIButton>(e);
                if (btn.hovered)
                {
                    firedAction = btn.actionId;
                    break;
                }
            }
        }
        wasDown = down;
    }
};