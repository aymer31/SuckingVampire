#pragma once

#include <SFML/Graphics.hpp>

#include "ecs/core.hpp"
#include "components.hpp"

#include <vector>
#include <string>

struct UITransform
{
    float x{}, y{};
};
struct UIRect
{
    float width{}, height{};
};
struct UICircle
{
    float radius{};
};

struct UIButton
{
    int actionId{};
    bool hovered{false};
    bool selected{false};
};

struct UILabel
{
    std::string text;
    unsigned charSize{24};
};
struct UITextColor
{
    sf::Color color{sf::Color::Red};
};

class UIButtonSpawnerSystem
{
public:
    struct ButtonSpec
    {
        int actionId{};
        bool isCircle{false};

        float x{}, y{};
        float width{220.f}, height{70.f};
        float radius{50.f};

        std::string label{"Button"};
        unsigned charSize{24};

        sf::Color textColor{sf::Color::White};
    };

    std::vector<ecs::Entity> created;

    void spawn(const std::vector<ButtonSpec>& specs)
    {
        created.clear();
        created.reserve(specs.size());

        for (const auto& s : specs)
        {
            ecs::Entity e = ecs::create_entity();
            created.push_back(e);

            ecs::add_components(e, UITransform{s.x, s.y}, UIButton{s.actionId, false, false},
                                UILabel{s.label, s.charSize}, UITextColor{s.textColor});

            if (s.isCircle)
                ecs::add_components(e, UICircle{s.radius});
            else
                ecs::add_components(e, UIRect{s.width, s.height});
        }
    }

    void destroy_created()
    {
        for (auto e : created)
            ecs::destroy_entity(e);
        created.clear();
    }
};