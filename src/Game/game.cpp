#include "Game/game.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Common/common.h"
#include <iostream>
#include <vector>
#include <cmath>

enum Direction
{
    Down = 0,
    Right = 1,
    Left = 2,
    Up = 3
};

struct SpriteRect
{
    int x;
    int y;
    int width;
    int height;
};

SpriteRect getSpriteRect(Direction direction, int frame, int spriteWidth, int spriteHeight)
{
    return {frame * spriteWidth, static_cast<int>(direction) * spriteHeight, spriteWidth, spriteHeight};
}

struct Player
{
    sf::RectangleShape shape;
    sf::Texture texture;
    sf::Sprite sprite;

    bool hasSprite = false;

    int currentFrame = 0;
    Direction currentDirection = Down;

    sf::Clock animationClock;
    float animationDelay = 0.15f;

    int maxHealth = 100;
    int health = 100;

    sf::RectangleShape healthBarBack;
    sf::RectangleShape healthBar;

    void initiatePlayer()
    {
        shape.setSize(sf::Vector2f(75.f, 75.f));
        shape.setPosition((widthWindow / 2.f) - 75.f, (heightWindow / 2.f) - 37.5f);

        if (texture.loadFromFile("assets/characterset.png"))
        {
            sprite.setTexture(texture);
            sprite.setPosition(shape.getPosition());
            sprite.setScale(shape.getSize().x / 32.f, shape.getSize().y / 32.f);
            hasSprite = true;
            updateSprite();
        }
        else
        {
            std::cout << "Error loading player texture" << std::endl;
        }

        healthBarBack.setSize(sf::Vector2f(75.f, 8.f));
        healthBarBack.setFillColor(sf::Color(50, 50, 50));
        healthBarBack.setPosition(shape.getPosition().x, shape.getPosition().y - 12.f);

        healthBar.setSize(sf::Vector2f(75.f, 8.f));
        healthBar.setFillColor(sf::Color::Green);
        healthBar.setPosition(shape.getPosition().x, shape.getPosition().y - 12.f);
    }

    void updateSprite()
    {
        if (!hasSprite)
            return;

        constexpr int SPRITE_WIDTH = 32;
        constexpr int SPRITE_HEIGHT = 32;

        SpriteRect rect = getSpriteRect(currentDirection, currentFrame, SPRITE_WIDTH, SPRITE_HEIGHT);
        sprite.setTextureRect(sf::IntRect(rect.x, rect.y, rect.width, rect.height));
    }

    void updateAnimation()
    {
        if (animationClock.getElapsedTime().asSeconds() >= animationDelay)
        {
            currentFrame = (currentFrame + 1) % 4;
            updateSprite();
            animationClock.restart();
        }
    }

    void updateHealthBar()
    {
        float ratio = static_cast<float>(health) / maxHealth;
        healthBar.setSize(sf::Vector2f(75.f * ratio, 8.f));

        if (ratio > 0.6f)
            healthBar.setFillColor(sf::Color::Green);
        else if (ratio > 0.3f)
            healthBar.setFillColor(sf::Color::Yellow);
        else
            healthBar.setFillColor(sf::Color::Red);

        healthBarBack.setPosition(shape.getPosition().x, shape.getPosition().y - 12.f);
        healthBar.setPosition(shape.getPosition().x, shape.getPosition().y - 12.f);
    }

    void takeDamage(int amount)
    {
        health -= amount;
        if (health < 0)
            health = 0;
        updateHealthBar();
    }

    void heal(int amount)
    {
        health += amount;
        if (health > maxHealth)
            health = maxHealth;
        updateHealthBar();
    }

    void moveBy(float dx, float dy)
    {
        shape.move(dx, dy);
        if (hasSprite)
            sprite.move(dx, dy);

        updateHealthBar();
    }

    void draw(sf::RenderWindow& window)
    {
        if (hasSprite)
            window.draw(sprite);
        else
            window.draw(shape);

        window.draw(healthBarBack);
        window.draw(healthBar);
    }
};

struct Enemy
{
    sf::CircleShape shape;

    Enemy(sf::Vector2f pos)
    {
        shape.setRadius(20.f);
        shape.setFillColor(sf::Color::Magenta);
        shape.setOrigin(20.f, 20.f);
        shape.setPosition(pos);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }
};

struct Meteor
{
    sf::CircleShape shape;
    sf::Vector2f target;
    float speed = 4.f;
    bool exploded = false;

    float damageRadius = 80.f;

    Meteor(sf::Vector2f spawn, sf::Vector2f targetPos)
        : target(targetPos)
    {
        shape.setRadius(15.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(15.f, 15.f);
        shape.setPosition(spawn);
    }

    bool update()
    {
        sf::Vector2f dir = target - shape.getPosition();
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (len < 3.f)
        {
            exploded = true;
            return true;
        }

        dir /= len;
        shape.move(dir * speed);
        return false;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }
};

float distance(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void getplayerMovement(Player& player)
{
    constexpr float speed = 1.f;
    bool isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
    {
        player.moveBy(0.f, -speed);
        player.currentDirection = Up;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        player.moveBy(0.f, speed);
        player.currentDirection = Down;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        player.moveBy(-speed, 0.f);
        player.currentDirection = Left;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        player.moveBy(speed, 0.f);
        player.currentDirection = Right;
        isMoving = true;
    }

    if (isMoving)
        player.updateAnimation();
    else
    {
        player.currentFrame = 0;
        player.updateSprite();
    }
}

void run_game()
{
    sf::RenderWindow game(sf::VideoMode(widthWindow, heightWindow), "Game", sf::Style::Default);
    game.setFramerateLimit(120);

    sf::View camera(sf::FloatRect(0.f, 0.f, widthWindow, heightWindow));

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("assets/background.png");
    backgroundTexture.setRepeated(true);

    sf::Sprite background;
    background.setTexture(backgroundTexture);
    background.setTextureRect(sf::IntRect(0, 0, 5000, 5000));

    Player player;
    player.initiatePlayer();

    std::vector<Meteor> meteors;
    std::vector<Enemy> enemies;

    sf::Clock meteorClock;
    float meteorDelay = 2.5f;

    while (game.isOpen())
    {
        sf::Event gameEvent;
        while (game.pollEvent(gameEvent))
        {
            if (gameEvent.type == sf::Event::Closed)
                game.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                game.close();
        }

        getplayerMovement(player);

        // Camera suit le joueur
        camera.setCenter(player.shape.getPosition());
        game.setView(camera);

        // ===== SPAWN METEOR AUTOUR DU JOUEUR =====
        if (meteorClock.getElapsedTime().asSeconds() >= meteorDelay && enemies.size() < 10)
        {
            float x = player.shape.getPosition().x + (rand() % 600 - 300);
            float y = player.shape.getPosition().y + (rand() % 600 - 300);

            meteors.emplace_back(sf::Vector2f(x, y - 300.f), // spawn au-dessus
                                 sf::Vector2f(x, y)          // impact
            );

            meteorClock.restart();
        }

        // ===== UPDATE METEORS =====
        for (int i = 0; i < meteors.size();)
        {
            bool arrived = meteors[i].update();

            if (arrived)
            {
                if (distance(player.shape.getPosition(), meteors[i].target) <= meteors[i].damageRadius)
                    player.takeDamage(25);

                if (enemies.size() < 10)
                    enemies.emplace_back(meteors[i].target);

                meteors.erase(meteors.begin() + i);
            }
            else
                ++i;
        }
        game.clear();
        game.draw(background);

        for (auto& m : meteors)
            m.draw(game);

        for (auto& e : enemies)
            e.draw(game);

        player.draw(game);

        game.display();
    }
}

struct projectile
{
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;

    projectile(sf::Vector2f pos, sf::Vector2f dir, float spd)
        : direction(dir)
        , speed(spd)
    {
        shape.setRadius(5.f);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(pos);
    }

    void update()
    {
        shape.move(direction * speed);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }
};

void handleProjectiles(std::vector<projectile>& projectiles, sf::RenderWindow& window)
{
    for (auto& proj : projectiles)
    {
        proj.update();
        proj.draw(window);
    }
}
