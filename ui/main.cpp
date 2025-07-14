#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <vector>

#include <thread>
#include <atomic>
#include <mutex>

#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>


std::vector<std::vector<sf::Vertex>> strokes;
std::string predictionResult;

std::mutex strokesMutex;
std::mutex predictionMutex;

std::atomic<bool> running(true);
std::atomic<bool> hasNewPrediction(true);
std::atomic<bool> strokesChanged(false);


// Function to get JSON response from readBuffer, returns JSON
nlohmann::json getJsonResponse(const std::string& readBuffer) {
    try {
        return nlohmann::json::parse(readBuffer);
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return nlohmann::json();
    }
}


// Callback function to write received data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function will POST to API and get response
void processData() {

    while (running) {
        {
            std::lock_guard<std::mutex> lock(strokesMutex);

            // if strokes has changed
            if (strokesChanged.load()) {
                std::cout << "Update to strokes occurred :: " << strokes.size() << std::endl;

                if (!strokes.empty()) {
                    // Do something with strokes
                    std::cout << "Processing " << strokes.size() << " strokes." << std::endl;
                }
                else{
                    std::cout << "No strokes to process." << std::endl;
                }


                CURL *curl = curl_easy_init();
                if(curl) {
                    std::string url = std::string(std::getenv("API_URL") ? std::getenv("API_URL") : "http://localhost:8000") + "/prediction";

                    std::string readBuffer;

                    // setting up curl options, received data will be written to readBuffer
                    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

                    CURLcode res = curl_easy_perform(curl);
                    if(res != CURLE_OK) {
                        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                        
                    } 
                    else {
                        // Simulate processing the response
                        nlohmann::json jsonResponse = getJsonResponse(readBuffer);
                        if(!jsonResponse.empty()) {
                            std::string message = jsonResponse.value("prediction", "No message in response");

                            std::cout << "API request processed successfully. ";
                            std::cout << "JSON message :: " << message << std::endl;

                            {
                                std::lock_guard<std::mutex> lock(predictionMutex);
                                predictionResult = message;
                                hasNewPrediction = true;
                            }
                        }
                        else {
                            std::cerr << "Received empty JSON response." << std::endl;
                            hasNewPrediction = false;
                        }

                    }

                    curl_easy_cleanup(curl);
                }

                strokesChanged.store(false);

            }

        }
    }
}

int main() {
    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 300;

    sf::RenderWindow window(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Freehand To Text", sf::Style::Close | sf::Style::Titlebar);
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
                        strokesChanged.store(true);
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
                            strokesChanged.store(true);
                        }
                        break;
                    
                    case sf::Keyboard::Scancode::Z: // Undo last stroke
                        {
                            std::lock_guard<std::mutex> lock(strokesMutex);
                            if (!strokes.empty()) {
                                strokes.pop_back();
                                strokesChanged.store(true);
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

        // Color the DRAWING area background
        sf::RectangleShape drawingBackground(sf::Vector2f(WIN_WIDTH, WIN_HEIGHT / 2));
        drawingBackground.setPosition({0, 0});
        drawingBackground.setFillColor(sf::Color(30, 30, 40)); // Dark blue-gray
        window.draw(drawingBackground);

        // Color the TEXT area background
        sf::RectangleShape textBackground(sf::Vector2f(WIN_WIDTH, WIN_HEIGHT / 2));
        textBackground.setPosition({0, WIN_HEIGHT / 2});
        textBackground.setFillColor(sf::Color::White);
        window.draw(textBackground);

        // Display the prediction result
        {
            std::lock_guard<std::mutex> lock(predictionMutex);
            if (hasNewPrediction.load()) {
                sf::Font font;

                if (!font.openFromFile("assets/Futura.ttc")) 
                {
                    // Handle error if font loading fails
                    std::cerr << "Font not found" << std::endl;
                    return -1;
                }

                sf::Text predictionText(font); // a font is required to make a text object
                predictionText.setString(predictionResult);
                predictionText.setCharacterSize(16);
                predictionText.setFillColor(sf::Color::Black);

                // ** needs to be centered in the text area, regardless of the text length
                predictionText.setPosition({WIN_WIDTH / 2 - 20, WIN_HEIGHT / 2 + 10}); // Position in the text area
                window.draw(predictionText);

            }
        }

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



