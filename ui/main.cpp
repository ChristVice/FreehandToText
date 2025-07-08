#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

#include <thread>
#include <atomic>
#include <mutex>

#include <curl/curl.h>
#include <string>

std::vector<std::vector<sf::Vertex>> strokes;
std::mutex strokesMutex;
std::atomic<bool> running(true);
std::atomic<bool> apiRequestPending(true);

// Function will POST to API and get response
void processData() {
    while (running) {
        {
            if (apiRequestPending) {
                // Simulate API request processing
                std::lock_guard<std::mutex> lock(strokesMutex);


                CURL *curl = curl_easy_init();
                if(curl) {
                    std::string url = std::string(std::getenv("API_URL") ? std::getenv("API_URL") : "http://localhost:8000") + "/prediction";

                    CURLcode res;
                    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

                    res = curl_easy_perform(curl);

                    if(res != CURLE_OK) {
                        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                    } 
                    else {
                        char *ct;
                        /* ask for the content-type */
                        res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
                    
                        if((CURLE_OK == res) && ct)
                            printf("We received Content-Type: %s\n", ct);
                        std::cout << typeid(res).name() << std::endl;
                        // Simulate processing the response
                        std::cout << "API request processed successfully." << std::endl;
                    }

                    curl_easy_cleanup(curl);
                }


                // Example: process strokes here
                // (replace with your CNN-LSTM preprocessing logic)
                if (!strokes.empty()) {
                    // Do something with strokes
                    std::cout << "Processing " << strokes.size() << " strokes." << std::endl;
                }
                else{
                    std::cout << "No strokes to process." << std::endl;
                }
            }
        }
    }
}

int main() {
    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 300;

    sf::RenderWindow window(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Freehand To Text");
    sf::IntRect drawingArea({0, 0}, {WIN_WIDTH, WIN_HEIGHT / 2});

    std::vector<sf::Vertex> currentStroke;
    bool drawing = false;

    // Start the data processing thread
    std::thread workingThread(processData);

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            // user is starting to draw, so we clear the current stroke if any
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (drawingArea.contains(mousePos)) {
                        currentStroke.clear();
                        drawing = true;
                    }
                }
            }

            // user is drawing, so we add points to the current stroke
            else if (drawing && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (drawingArea.contains(mousePos)) {
                    currentStroke.push_back(sf::Vertex{sf::Vector2f(mousePos.x, mousePos.y), sf::Color::White});
                }
            }

            // user has released the mouse button, so we finalize the current stroke
            else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    drawing = false;
                    if (!currentStroke.empty()) {
                        std::lock_guard<std::mutex> lock(strokesMutex);
                        strokes.push_back(currentStroke);
                    }
                    currentStroke.clear();
                }
            }

            //  ****** Handle keyboard shortcuts ******
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch(keyPressed->scancode) {
                    case sf::Keyboard::Scancode::C: // Clear strokes
                        {
                            std::lock_guard<std::mutex> lock(strokesMutex);
                            strokes.clear();
                            currentStroke.clear();
                        }
                        break;
                    
                    case sf::Keyboard::Scancode::Z: // Undo last stroke
                        {
                            std::lock_guard<std::mutex> lock(strokesMutex);
                            if (!strokes.empty()) {
                                strokes.pop_back();
                            }
                        }
                        break;
                    
                    case sf::Keyboard::Scancode::Escape: // Close window
                        window.close();
                        break;
                    
                    default:
                        // Handle other keys if needed
                        break;
                }
            }
        }

        // background color
        window.clear(sf::Color::Black);

        // draw the border
        sf::RectangleShape divider(sf::Vector2f(WIN_WIDTH, 2));
        divider.setPosition({0, WIN_HEIGHT / 2});
        divider.setFillColor(sf::Color(100, 100, 100));
        window.draw(divider);

        // draw in the drawing area
        {
            std::lock_guard<std::mutex> lock(strokesMutex);
            for (const auto& stroke : strokes) {
                if (!stroke.empty())
                    window.draw(&stroke[0], stroke.size(), sf::PrimitiveType::LineStrip);
            }
        }

        // draw remaining current stroke 
        if (!currentStroke.empty())
            window.draw(&currentStroke[0], currentStroke.size(), sf::PrimitiveType::LineStrip);

        window.display();
    }

    running = false;
    workingThread.join();
    curl_global_cleanup();
    return 0;
}



