#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "Button.h"

#include "Button.h"  // Include the header

Button::Button(float x, float y, float width, float height, const std::string& buttonText, sf::Font& buttonFont) {
    this->xPos = x;
    this->yPos = y;
    this->font = buttonFont;
    this->buttonText = buttonText;
    this->width = width;
    this->height = height;

    shape.setPosition({x, y});
    shape.setSize({width, height});
    shape.setFillColor(sf::Color(70, 70, 70));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color(100, 100, 100));

}

void Button::draw(sf::RenderWindow& window) {
    sf::Text text(font);
    text.setString(buttonText);
    text.setCharacterSize(10);
    text.setFillColor(sf::Color::White);

    sf::FloatRect shapeBounds = shape.getLocalBounds();
    sf::FloatRect textBounds = text.getLocalBounds();

    float centeredX = xPos+shapeBounds.getCenter().x-textBounds.getCenter().x;
    float centeredY = yPos+shapeBounds.getCenter().y-textBounds.getCenter().y;

    text.setPosition({
        std::round(centeredX),
        std::round(centeredY)
    });

    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked(sf::Vector2i mousePos) {
    return this->shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

bool Button::isHovering(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setPressed(bool pressed) {
    isPressed = pressed;
    if (pressed) {
        this->shape.setFillColor(sf::Color(90, 90, 90));
    } else {
        this->shape.setFillColor(sf::Color(70, 70, 70));
    }
}