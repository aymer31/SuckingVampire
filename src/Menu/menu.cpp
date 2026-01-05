#include "Menu/menu.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Common/common.h"
#include "Game/game.h"




void run_menu(){
    sf::RenderWindow window(sf::VideoMode(widthWindow, heightWindow), "Menu", sf::Style::Default);
              while (window.isOpen()) {
                  sf::Event event;
                  while (window.pollEvent(event))
                  {
                      if (event.type == sf::Event::Closed)
                      {
                          window.close();
                      }

                      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                      {
                          window.close();
                      }
                  }
                  window.clear(sf::Color::White);

                  sf::CircleShape Exit_button(50.f);
                  Exit_button.setFillColor(sf::Color::Black);
                  Exit_button.setOutlineThickness(2.f);
                  Exit_button.setOutlineColor(sf::Color::Red);
                  Exit_button.setPosition((widthWindow / 2) + 100, heightWindow / 2);

                  sf::RectangleShape Start_button(sf::Vector2f(150.f, 75.f));
                  Start_button.setFillColor(sf::Color:: Black);
                  Start_button.setOutlineThickness(2.f);
                  Start_button.setOutlineColor(sf::Color::Black);
                  Start_button.setPosition((widthWindow / 2) - 100, heightWindow / 2);

                  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                  {
                      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                      if (Exit_button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                      {
                          window.close();

                      }
                      if (Start_button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                      {
                          run_game();
                          window.close();
                      }
                  }

                  window.draw(Start_button);
                  window.draw(Exit_button);
                  window.display();
              } window.setFramerateLimit(120);

}


// sf::RenderWindow create_window(const std::string& title)
//{
//     sf::RenderWindow window(sf::VideoMode(widthWindow, heightWindow), title, sf::Style::Default);
//     window.setFramerateLimit(120);
//     return window;
// }