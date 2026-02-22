#include "Menu/menu.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Common/common.h"
#include "Game/game.h"
#include "ecs/components.hpp"
#include "ecs/UIButton_system.hpp"
#include "ecs/UIRender_system.hpp"
#include "ecs/UIInput_system.hpp"           
#include <iostream>
#include <memory>

namespace MenuAction
{
constexpr int Start = 1;
constexpr int Settings = 2;
constexpr int Exit = 3;
}

void run_menu()
{
    sf::RenderWindow window(sf::VideoMode(widthWindow, heightWindow), "Menu");
    window.setFramerateLimit(120);

    sf::Font font;
    bool fontOk = font.loadFromFile("assets/arial.ttf");
    std::cout << "fontOk=" << fontOk << "\n";
    std::cout << "cwd guess: assets/arial.ttf\n";
    if (!fontOk)
        std::cerr << "[Menu] Police introuvable: assets/arial.ttf\n";

    ecs::register_component<UITransform>();
    ecs::register_component<UIRect>();
    ecs::register_component<UICircle>();
    ecs::register_component<UIButton>();
    ecs::register_component<UILabel>();
    ecs::register_component<UITextColor>();

    auto inputSys = std::make_shared<UIInputSystem>();
    auto renderSys = std::make_shared<UIRenderSystem>();

    renderSys->setFont(&font);
    std::cout << "[Menu] fontOk = " << fontOk << "\n";

    ecs::Signature uiSig = ecs::create_signature<UITransform, UIButton>();
    ecs::register_system(inputSys, uiSig);
    ecs::register_system(renderSys, uiSig);

    UIButtonSpawnerSystem spawner;

    const float bx = widthWindow / 2.f - 110.f;
    const float by = heightWindow / 2.f - 120.f;
    const float bw = 220.f;
    const float bh = 70.f;

    spawner.spawn({{MenuAction::Start, false, bx, by, bw, bh, 0.f, "START", 26},
                   {MenuAction::Settings, false, bx, by + 90.f, bw, bh, 0.f, "SETTINGS", 22},
                   {MenuAction::Exit, false, bx, by + 180.f, bw, bh, 0.f, "EXIT", 26}});

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        inputSys->update(window);

        if (inputSys->firedAction)
        {
            switch (*inputSys->firedAction)
            {
                case MenuAction::Start:
                    spawner.destroy_created(); 
                    window.close();
                    std::cout << "[Menu] calling run_game()\n";
                    run_game();
                    return;

                case MenuAction::Settings:
                    std::cout << "[Menu] Settings clicked (TODO)\n";
                    break;

                case MenuAction::Exit:
                    spawner.destroy_created();
                    window.close();
                    return;
            }
        }

        window.clear(sf::Color::White);
        renderSys->render(window);
        window.display();
    }

    spawner.destroy_created();
}