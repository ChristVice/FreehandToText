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

    public:
        Button(float x, float y, float width, float height, const std::string& buttonText, sf::Font& buttonFont);

        void draw(sf::RenderWindow& window);
        bool isClicked(sf::Vector2i mousePos);
        bool isHovering(sf::Vector2i mousePos);
        void setPressed(bool pressed);

};