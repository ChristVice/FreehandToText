#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "Button.h"

#include "Button.h"  // Include the header

Button::Button(float x, float y, float width, float height, const std::string& buttonText, sf::Font& buttonFont) 
    : xPos(x), yPos(y), width(width), height(height), buttonText(buttonText), font(buttonFont), isPressed(false)
{
    shape.setPosition({x, y});
    shape.setSize({width, height});
    shape.setFillColor(BUTTON_BKG_COLOR);
}

void Button::draw(sf::RenderWindow& window, sf::Color textColor, sf::Color highlightTextColor) {
    sf::Text text(font);
    text.setString(buttonText);
    text.setCharacterSize(12);

    if(this->isPressed) text.setFillColor(highlightTextColor);
    else                text.setFillColor(textColor);

    sf::FloatRect shapeBounds = shape.getLocalBounds();
    sf::FloatRect textBounds = text.getLocalBounds();

    float centeredX = xPos+shapeBounds.getCenter().x-textBounds.getCenter().x;
    float centeredY = yPos+shapeBounds.getCenter().y-textBounds.getCenter().y;

    text.setPosition({ std::round(centeredX), std::round(centeredY) });

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

    this->isPressed = pressed;

    if (pressed)    this->shape.setFillColor(BUTTON_BKG_HIGHLIGHT_COLOR);
    else            this->shape.setFillColor(BUTTON_BKG_COLOR);
    
}