#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
    private:
        sf::RectangleShape shape;
        sf::Font font;
        std::string buttonText;
        float xPos, yPos, width, height;
        bool isPressed;
        int cornerRadius;

        const sf::Color BUTTON_BKG_COLOR{0x4B, 0x4B, 0x4C};
        const sf::Color BUTTON_BKG_HIGHLIGHT_COLOR{0x43, 0x43, 0x45};

    public:
        Button(float x, float y, float width, float height,
               const std::string& buttonText, sf::Font& buttonFont);

        void draw(sf::RenderWindow& window, sf::Color textColor, sf::Color highlightTextColor);
        bool isClicked(sf::Vector2i mousePos);
        bool isHovering(sf::Vector2i mousePos);
        void setPressed(bool pressed);

};