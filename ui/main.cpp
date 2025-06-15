#include <SFML/Graphics.hpp>
#include <vector>

int main() {

    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 300;

    sf::RenderWindow window(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Freehand Drawing To Text");

    // Define drawing area (top half)
    sf::IntRect drawingArea({0, 0}, {WIN_WIDTH, WIN_HEIGHT / 2});

    // Draw a horizontal line to split the screen in the middle
    sf::RectangleShape divider(sf::Vector2f(WIN_WIDTH, 2));
    divider.setPosition({0, WIN_HEIGHT / 2});
    divider.setFillColor(sf::Color(100, 100, 100));

    // Store all strokes (each stroke is a vector of points)
    std::vector<std::vector<sf::Vertex>> strokes;
    std::vector<sf::Vertex> currentStroke;
    bool drawing = false;

    while (window.isOpen()) {

        // handle events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Start a new stroke on mouse press (only if in drawing area)
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {

                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (drawingArea.contains(mousePos)) {
                        currentStroke.clear();
                        drawing = true;
                    }
                }

            }

            // Finish the stroke on mouse release
            if(const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {

                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    drawing = false;
                    if (!currentStroke.empty()) {
                        strokes.push_back(currentStroke);
                        currentStroke.clear();
                    }
                }

            }

            // Checks for key presses
            if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                // Undo last stroke with 'Z'
                if (keyPressed->code == sf::Keyboard::Key::Z && !strokes.empty()) 
                    strokes.pop_back();

                // Close window with 'Escape'
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();

                // Clear all strokes with 'C'
                if (keyPressed->code == sf::Keyboard::Key::C) {
                    strokes.clear();
                    currentStroke.clear();
                }

            }
        }

        // Add points while mouse is held down (only if in drawing area)
        if (drawing && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (drawingArea.contains(mousePos)) {
                currentStroke.push_back({sf::Vector2f(mousePos.x, mousePos.y), sf::Color::White});
            }
        }

        window.clear(sf::Color(0x21, 0x25, 0x29)); // #212529 background
        window.draw(divider);

        // Draw all completed strokes (only in drawing area)
        for (const auto& stroke : strokes) {
            if (!stroke.empty())
                window.draw(stroke.data(), stroke.size(), sf::PrimitiveType::LineStrip);
        }

        // Draw the current stroke in progress
        if (!currentStroke.empty())
            window.draw(currentStroke.data(), currentStroke.size(), sf::PrimitiveType::LineStrip);

        window.display();
    }

    return 0;
}



