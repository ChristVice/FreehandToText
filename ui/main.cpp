#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

#include <thread>
#include <atomic>
#include <mutex>

std::vector<std::vector<sf::Vertex>> strokes;
std::mutex strokesMutex;
std::atomic<bool> running(true);
std::atomic<bool> apiRequestPending(true);

void processData() {
    while (running) {
        {
            if (apiRequestPending) {
                // Simulate API request processing
                std::lock_guard<std::mutex> lock(strokesMutex);
                // Example: process strokes here
                // (replace with your CNN-LSTM preprocessing logic)
                if (!strokes.empty()) {
                    // Do something with strokes
                    std::cout << "Processing " << strokes.size() << " strokes." << std::endl;
                }
            }
        }
    }
}

int main() {
    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 300;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Freehand To Text");
    sf::IntRect drawingArea(0, 0, WIN_WIDTH, WIN_HEIGHT / 2);

    std::vector<sf::Vertex> currentStroke;
    bool drawing = false;

    // Start the data processing thread
    std::thread worker(processData);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (drawingArea.contains(mousePos)) {
                    currentStroke.clear();
                    drawing = true;
                }
            }

            if (drawing && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (drawingArea.contains(mousePos)) {
                    currentStroke.push_back(sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y), sf::Color::White));
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                drawing = false;
                if (!currentStroke.empty()) {
                    std::lock_guard<std::mutex> lock(strokesMutex);
                    strokes.push_back(currentStroke);
                    currentStroke.clear();
                }
            }

            // Handle keyboard shortcuts
            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::C: // when C is pressed, clear the strokes
                        {
                            std::lock_guard<std::mutex> lock(strokesMutex);
                            strokes.clear();
                            currentStroke.clear();
                        }
                        break;
                    
                    case sf::Keyboard::Z: // when Z is pressed, undo the last stroke
                        {
                            std::lock_guard<std::mutex> lock(strokesMutex);
                            if(!strokes.empty()) {
                                strokes.pop_back();
                            }
                        }
                        break;
                    
                    case sf::Keyboard::Escape: // when Escape is pressed, close the window
                        window.close();
                        break;
                }
            }
        }

        window.clear(sf::Color::Black);

        sf::RectangleShape divider(sf::Vector2f(WIN_WIDTH, 2));
        divider.setPosition(0, WIN_HEIGHT / 2);
        divider.setFillColor(sf::Color(100, 100, 100));
        window.draw(divider);

        {
            std::lock_guard<std::mutex> lock(strokesMutex);
            for (const auto& stroke : strokes) {
                if (!stroke.empty())
                    window.draw(&stroke[0], stroke.size(), sf::LineStrip);
            }
        }

        if (!currentStroke.empty())
            window.draw(&currentStroke[0], currentStroke.size(), sf::LineStrip);

        window.display();
    }

    running = false;
    worker.join();
    return 0;
}



