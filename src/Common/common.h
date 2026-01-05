#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>


constexpr int widthWindow = 2000;
constexpr int heightWindow = 1000;

void center_horizontally(const auto& parent, auto& child)
{
    const auto parentBoundingBox = parent.getLocalBounds();
    const sf::Vector2f parentLocalCenter{.x = parentBoundingBox.left + parentBoundingBox.width * 0.5f,
                                         .y = parentBoundingBox.top + parentBoundingBox.height * 0.5f};

    const sf::Vector2f parentGlobalCenter = parent.getTransform().transformPoint(parentLocalCenter);

    const auto childBoundingBox = child.getLocalBounds();
    const auto childOrigin = child.getOrigin();
    child.setOrigin(childBoundingBox.left + childBoundingBox.width * 0.5f, childOrigin.y);

    const auto childPosition = child.getPosition();
    child.setPosition(parentGlobalCenter.x, childPosition.y);
}

void center_vertically(const auto& parent, auto& child)
{
    const auto parentBoundingBox = parent.getLocalBounds();
    const sf::Vector2f parentLocalCenter{.x = parentBoundingBox.left + parentBoundingBox.width * 0.5f,
                                         .y = parentBoundingBox.top + parentBoundingBox.height * 0.5f};

    const sf::Vector2f parentGlobalCenter = parent.getTransform().transformPoint(parentLocalCenter);

    const auto childBoundingBox = child.getLocalBounds();
    const auto childOrigin = child.getOrigin();
    child.setOrigin(childOrigin.x, childBoundingBox.top + childBoundingBox.height * 0.5f);

    const auto childPosition = child.getPosition();
    child.setPosition(childPosition.x, parentGlobalCenter.y);
}
