#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

int main() {

    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 300;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Freehand To Text");

    // Define drawing area (top half)
    sf::IntRect drawingArea(0, 0, WIN_WIDTH, WIN_HEIGHT / 2);

    // Store all strokes (each stroke is a vector of points)
    std::vector<std::vector<sf::Vertex>> strokes;
    std::vector<sf::Vertex> currentStroke;
    bool drawing = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Start a new stroke on mouse press (only if in drawing area)
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (drawingArea.contains(mousePos)) {
                    currentStroke.clear();
                    drawing = true;
                }
            }

            // Add points while mouse is held down (only if in drawing area)
            if (drawing && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (drawingArea.contains(mousePos)) {
                    currentStroke.push_back(sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y), sf::Color::White));
                }
            }

            // Finish the stroke on mouse release
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                drawing = false;
                if (!currentStroke.empty()) {
                    strokes.push_back(currentStroke);
                    currentStroke.clear();
                }
            }

            // Clear all strokes with 'C'
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
                strokes.clear();
                currentStroke.clear();
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) {
                if(!strokes.empty()) {
                    strokes.pop_back();
                }
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        // Draw a horizontal line to split the screen in the middle
        sf::RectangleShape divider(sf::Vector2f(WIN_WIDTH, 2));
        divider.setPosition(0, WIN_HEIGHT / 2);
        divider.setFillColor(sf::Color(100, 100, 100));
        window.draw(divider);

        // Draw all completed strokes (only in drawing area)
        for (const auto& stroke : strokes) {
            if (!stroke.empty())
                window.draw(&stroke[0], stroke.size(), sf::LineStrip);
        }

        // Draw the current stroke in progress
        if (!currentStroke.empty())
            window.draw(&currentStroke[0], currentStroke.size(), sf::LineStrip);

        window.display();
    }

    return 0;
}



