#pragma once
#include "ecs/core.hpp"
#include "components.hpp"
#include <SFML/Graphics.hpp>

class UIRenderSystem : public ecs::System
{
public:
    void setFont(const sf::Font* font)
    {
        _font = font;
    }

    sf::Color fillNormal = sf::Color::Black;
    sf::Color fillHover = sf::Color(50, 50, 50);
    sf::Color outlineNormal = sf::Color::Black;
    sf::Color outlineSelected = sf::Color::Yellow;
    sf::Color textColor = sf::Color::White;
    float outlineThickness = 2.f;

    void render(sf::RenderWindow& window)
    {
        for (auto e : entities())
        {
            const auto& tr = ecs::get_component<UITransform>(e);
            const auto& btn = ecs::get_component<UIButton>(e);

            const sf::Color fill = btn.hovered ? fillHover : fillNormal;
            const sf::Color outline = btn.selected ? outlineSelected : outlineNormal;

            if (ecs::has_component<UIRect>(e))
            {
                const auto& rc = ecs::get_component<UIRect>(e);

                sf::RectangleShape shape({rc.width, rc.height});
                shape.setPosition(tr.x, tr.y);
                shape.setFillColor(fill);
                shape.setOutlineThickness(outlineThickness);
                shape.setOutlineColor(outline);

                window.draw(shape);
                drawLabel(window, e, shape.getGlobalBounds());
            }
            else if (ecs::has_component<UICircle>(e))
            {
                const auto& cc = ecs::get_component<UICircle>(e);

                sf::CircleShape shape(cc.radius);
                shape.setPosition(tr.x, tr.y);
                shape.setFillColor(fill);
                shape.setOutlineThickness(outlineThickness);
                shape.setOutlineColor(outline);

                window.draw(shape);
                drawLabel(window, e, shape.getGlobalBounds());
            }
        }
    }

private:
    void drawLabel(sf::RenderWindow& window, ecs::Entity e, const sf::FloatRect& bounds)
    {
        if (!_font)
            return;
        if (!ecs::has_component<UILabel>(e))
            return;

        const auto& label = ecs::get_component<UILabel>(e);

        sf::Text text;
        text.setFont(*_font);
        text.setString(label.text);
        text.setCharacterSize(label.charSize);
        sf::Color c = textColor;
        if (ecs::has_component<UITextColor>(e))
            c = ecs::get_component<UITextColor>(e).color;
        text.setFillColor(c);

        auto tb = text.getLocalBounds();
        text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        text.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

        window.draw(text);
    }

private:
    const sf::Font* _font{nullptr};
};