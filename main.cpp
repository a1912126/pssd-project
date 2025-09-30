#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <tuple>

enum Screen { HOME, MAIN };

const int GRID_WIDTH = 900;
const int GRID_HEIGHT = 600;
const int TILE_SIZE = 1;

// Function to read the map from "map.txt"
std::vector<std::vector<int>> readMapFromFile(const std::string& filename) {
    std::vector<std::vector<int>> cityMap;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream ss(line);
        int value;
        while (ss >> value) {
            row.push_back(value);
        }
        cityMap.push_back(row);
    }
    return cityMap;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Screen Switching App");

    Screen currentScreen = HOME;

    // Load font once
    sf::Font font;
    if (!font.loadFromFile("Bitcount.ttf")) {
        return -1;
    }

    // Home Screen UI
    sf::Text homeText("Home Screen", font, 50);
    homeText.setFillColor(sf::Color::White);
    homeText.setPosition(350, 250);

    sf::Text homeHint("Press ENTER to start", font, 24);
    homeHint.setFillColor(sf::Color::White);
    homeHint.setPosition(350, 320);

    // Main Screen UI
    sf::Text mainText("Main Screen", font, 50);
    mainText.setFillColor(sf::Color::White);
    mainText.setPosition(350, 250);

    sf::Text mainHint("Press ESC to return", font, 24);
    mainHint.setFillColor(sf::Color::White);
    mainHint.setPosition(350, 320);

    // Instruction for selecting start/end point
    sf::Text selectPointText("", font, 28);
    selectPointText.setFillColor(sf::Color::Yellow);
    selectPointText.setPosition(20, 20);

    // Confirm button
    sf::RectangleShape confirmButton(sf::Vector2f(180, 50));
    confirmButton.setFillColor(sf::Color(100, 200, 100));
    confirmButton.setPosition(500, 20);

    sf::Text confirmText("Confirm", font, 28);
    confirmText.setFillColor(sf::Color::Black);
    confirmText.setPosition(
        500 + (180 - confirmText.getLocalBounds().width) / 2,
        20 + (50 - confirmText.getLocalBounds().height) / 2 - 8
    );

    // Restart button
    sf::RectangleShape restartButton(sf::Vector2f(180, 50));
    restartButton.setFillColor(sf::Color(200, 100, 100));
    restartButton.setPosition(700, 20);

    sf::Text restartText("START AGAIN", font, 24);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(
        700 + (180 - restartText.getLocalBounds().width) / 2,
        20 + (50 - restartText.getLocalBounds().height) / 2 - 8
    );

    // For your map logic
    std::vector<std::vector<int>> cityMap = readMapFromFile("map.txt");

    // Variables for your game logic
    bool startPointSelected = false;
    bool startPointConfirmed = false;
    bool endPointSelected = false;
    bool endPointConfirmed = false;
    sf::Vector2i startPoint;
    sf::Vector2i endPoint;

    // Pathfinding variables
    std::vector<std::vector<int>> pathMap;
    bool pathSearched = false;
    bool pathFound = false;
    std::queue<std::tuple<int, int, int>> bfsQueue;
    bool bfsInProgress = false;
    sf::Clock bfsClock;
    float bfsDelay = 0.000002f; // seconds between steps, increase for slower

    // Directions: up, left, down, right
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, -1, 0, 1};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        if (currentScreen == HOME) {
            window.draw(homeText);
            window.draw(homeHint);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                currentScreen = MAIN;
                startPointSelected = false;
                startPointConfirmed = false;
                endPointSelected = false;
                endPointConfirmed = false;
                pathSearched = false;
                pathFound = false;
                bfsInProgress = false;
                while (!bfsQueue.empty()) bfsQueue.pop();
                window.clear(sf::Color::Black);
            }
        } else if (currentScreen == MAIN) {
            // Draw the white border rectangle (904x604)
            sf::RectangleShape whiteBorder(sf::Vector2f(904, 604));
            whiteBorder.setFillColor(sf::Color::White);
            whiteBorder.setPosition(50 - 2, 800 - 20 - GRID_HEIGHT - 2);

            // Draw the black inner rectangle (900x600)
            sf::RectangleShape blackRect(sf::Vector2f(GRID_WIDTH, GRID_HEIGHT));
            blackRect.setFillColor(sf::Color::Black);
            blackRect.setPosition(50, 800 - 20 - GRID_HEIGHT);

            window.draw(whiteBorder);
            window.draw(blackRect);

            // Draw the city map (walls only)
            for (int y = 0; y < cityMap.size(); ++y) {
                for (int x = 0; x < cityMap[y].size(); ++x) {
                    if (cityMap[y][x] == 1) {
                        sf::RectangleShape building(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                        building.setFillColor(sf::Color::White);
                        building.setPosition(50 + x * TILE_SIZE, 800 - 20 - GRID_HEIGHT + y * TILE_SIZE);
                        window.draw(building);
                    }
                }
            }

            // Draw start point if selected
            if (startPointSelected) {
                sf::CircleShape startMarker(8);
                startMarker.setFillColor(sf::Color::Red);
                startMarker.setPosition(50 + startPoint.x - 8, 800 - 20 - GRID_HEIGHT + startPoint.y - 8);
                window.draw(startMarker);
            }

            // Draw end point if selected
            if (endPointSelected) {
                sf::CircleShape endMarker(8);
                endMarker.setFillColor(sf::Color::Blue);
                endMarker.setPosition(50 + endPoint.x - 8, 800 - 20 - GRID_HEIGHT + endPoint.y - 8);
                window.draw(endMarker);
            }

            // --- Selection Logic ---
            // 1. Select Start Point
            if (!startPointConfirmed) {
                selectPointText.setString("SELECT A START POINT");
                window.draw(selectPointText);
                if (startPointSelected) {
                    window.draw(confirmButton);
                    window.draw(confirmText);
                }
                // Mouse logic for picking start point
                if (!startPointSelected && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = mousePos.x - 50;
                    int gridY = mousePos.y - (800 - 20 - GRID_HEIGHT);
                    sf::FloatRect btnRect = confirmButton.getGlobalBounds();
                    if (gridX >= 0 && gridX < GRID_WIDTH &&
                        gridY >= 0 && gridY < GRID_HEIGHT &&
                        !btnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        startPoint = sf::Vector2i(gridX, gridY);
                        startPointSelected = true;
                    }
                }
                // Confirm button logic
                if (startPointSelected && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect btnRect = confirmButton.getGlobalBounds();
                    if (btnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        startPointConfirmed = true;
                        std::cout << "Start point confirmed at: (" << startPoint.x << ", " << startPoint.y << ")" << std::endl;
                    }
                }
            }
            // 2. Select End Point
            else if (!endPointConfirmed) {
                selectPointText.setString("SELECT AN END POINT");
                window.draw(selectPointText);
                if (endPointSelected) {
                    window.draw(confirmButton);
                    window.draw(confirmText);
                }
                // Mouse logic for picking end point
                if (!endPointSelected && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = mousePos.x - 50;
                    int gridY = mousePos.y - (800 - 20 - GRID_HEIGHT);
                    sf::FloatRect btnRect = confirmButton.getGlobalBounds();
                    if (gridX >= 0 && gridX < GRID_WIDTH &&
                        gridY >= 0 && gridY < GRID_HEIGHT &&
                        !btnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        endPoint = sf::Vector2i(gridX, gridY);
                        endPointSelected = true;
                    }
                }
                // Confirm button logic
                if (endPointSelected && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect btnRect = confirmButton.getGlobalBounds();
                    if (btnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        endPointConfirmed = true;
                        std::cout << "End point confirmed at: (" << endPoint.x << ", " << endPoint.y << ")" << std::endl;
                    }
                }
            }
            // 3. Pathfinding after both points confirmed
            else if (startPointConfirmed && endPointConfirmed) {
                // Draw restart button
                window.draw(restartButton);
                window.draw(restartText);

                // Handle restart button click
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect btnRect = restartButton.getGlobalBounds();
                    if (btnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        // Reset everything
                        startPointSelected = false;
                        startPointConfirmed = false;
                        endPointSelected = false;
                        endPointConfirmed = false;
                        pathSearched = false;
                        pathFound = false;
                        bfsInProgress = false;
                        while (!bfsQueue.empty()) bfsQueue.pop();
                        continue;
                    }
                }

                // Initialize BFS only once
                if (!pathSearched && !bfsInProgress) {
                    pathMap = cityMap;
                    int rows = pathMap.size();
                    int cols = pathMap[0].size();
                    while (!bfsQueue.empty()) bfsQueue.pop();
                    bfsQueue.push({startPoint.y, startPoint.x, 0});
                    pathMap[startPoint.y][startPoint.x] = 0;
                    pathFound = false;
                    bfsInProgress = true;
                    bfsClock.restart();
                }

                // Step-by-step BFS (process only a few nodes per frame after delay)
                if (bfsInProgress && bfsClock.getElapsedTime().asSeconds() > bfsDelay) {
                    bfsClock.restart();
                    int rows = pathMap.size();
                    int cols = pathMap[0].size();
                    int stepsThisFrame = 10000; // Only process 1 node per frame for visible animation

                    for (int i = 0; i < stepsThisFrame && !bfsQueue.empty() && !pathFound; ++i) {
                        auto [r, c, steps] = bfsQueue.front();
                        bfsQueue.pop();

                        if (r == endPoint.y && c == endPoint.x) {
                            pathFound = true;
                            bfsInProgress = false;
                            pathSearched = true;
                            break;
                        }

                        for (int d = 0; d < 4; ++d) {
                            int nr = r + dr[d];
                            int nc = c + dc[d];
                            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && pathMap[nr][nc] == 0) {
                                pathMap[nr][nc] = -(steps + 1);
                                bfsQueue.push({nr, nc, steps + 1});
                            }
                        }
                    }
                    // If queue is empty and not found, mark as searched
                    if (bfsQueue.empty() && !pathFound) {
                        bfsInProgress = false;
                        pathSearched = true;
                    }
                }

                // Draw all visited cells as light yellow, except walls (1)
                for (int y = 0; y < pathMap.size(); ++y) {
                    for (int x = 0; x < pathMap[y].size(); ++x) {
                        if (pathMap[y][x] < 0) {
                            sf::RectangleShape visitedCell(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                            visitedCell.setFillColor(sf::Color(255, 255, 180)); // light yellow
                            visitedCell.setPosition(50 + x * TILE_SIZE, 800 - 20 - GRID_HEIGHT + y * TILE_SIZE);
                            window.draw(visitedCell);
                        }
                    }
                }

                // Draw the shortest path in green if found
                if (pathFound) {
                    int r = endPoint.y, c = endPoint.x;
                    int val = pathMap[r][c];
                    while (val < 0) {
                        sf::RectangleShape pathCell(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                        pathCell.setFillColor(sf::Color::Green);
                        pathCell.setPosition(50 + c * TILE_SIZE, 800 - 20 - GRID_HEIGHT + r * TILE_SIZE);
                        window.draw(pathCell);

                        // Find neighbor with value == val + 1 (closer to source)
                        bool found = false;
                        for (int d = 0; d < 4 && !found; ++d) {
                            int nr = r + dr[d];
                            int nc = c + dc[d];
                            if (nr >= 0 && nr < (int)pathMap.size() && nc >= 0 && nc < (int)pathMap[0].size()) {
                                if (pathMap[nr][nc] == val + 1) {
                                    r = nr;
                                    c = nc;
                                    val = pathMap[r][c];
                                    found = true;
                                }
                            }
                        }
                        if (!found) break;
                    }
                }
            }
            // ESC to go back to HOME
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                currentScreen = HOME;
                startPointSelected = false;
                startPointConfirmed = false;
                endPointSelected = false;
                endPointConfirmed = false;
                pathSearched = false;
                pathFound = false;
                bfsInProgress = false;
                while (!bfsQueue.empty()) bfsQueue.pop();
            }
        }

        window.display();
    }

    return 0;
}