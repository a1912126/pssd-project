#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Initial window
    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML Game");

    // Create an sf::Font object (use SFML's default font)
    sf::Font font;
    if (!font.loadFromFile("Bitcount.ttf")) {  
        return -1; // Failed to load font
    }

    // Set up the title text
    sf::Text title;
    title.setString("Game Title");
    title.setCharacterSize(50);        // Set the font size
    title.setFillColor(sf::Color::White); // Set the color to white
    title.setFont(font);  // Use the loaded font

    // Position the title in the center of the window
    title.setPosition(window.getSize().x / 2 - title.getLocalBounds().width / 2, 
                      window.getSize().y / 3 - title.getLocalBounds().height / 2);

    // Set up the "START" button
    sf::RectangleShape startButton(sf::Vector2f(200, 50)); // Create a rectangle for the button
    startButton.setFillColor(sf::Color::Green);  // Button color
    startButton.setPosition(window.getSize().x / 2 - startButton.getSize().x / 2, window.getSize().y / 2);  // Center the button

    // Set up the "START" text inside the button
    sf::Text startText;
    startText.setString("START");  // Set the string for the button text
    startText.setCharacterSize(30);  // Set the character size for the button text
    startText.setFillColor(sf::Color::Black);  // Text color
    startText.setFont(font);  // Use the same font as the title

    // Position the "START" text in the center of the button
    startText.setPosition(startButton.getPosition().x + startButton.getSize().x / 2 - startText.getLocalBounds().width / 2, 
                          startButton.getPosition().y + startButton.getSize().y / 2 - startText.getLocalBounds().height / 2 - 10);  // Center the text inside the button

    // Variable to track if we should switch to the next screen
    bool gameStarted = false;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Detect mouse click on the "START" button
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Check if the click is within the button's bounds
                    if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        // If clicked, start the game or transition to the next screen
                        std::cout << "START button clicked!" << std::endl;
                        gameStarted = true;
                    }
                }
            }
        }

        // Clear the screen (initial window)
        window.clear(sf::Color::Black);  // Optional: Clear to black background

        // Draw the title and button
        window.draw(title);
        window.draw(startButton);
        window.draw(startText);

        // If the game has started, create the black rectangle with a white border
        if (gameStarted) {
            // Create the white border (larger rectangle)
            sf::RectangleShape whiteBorder(sf::Vector2f(904, 604));  // 900 + 2px border on each side
            whiteBorder.setFillColor(sf::Color::White);  // White border color
            whiteBorder.setPosition(50 - 2, 800 - 20 - 600 - 2);  // Position with 2px padding outside

            // Create the black inner rectangle (smaller)
            sf::RectangleShape blackRect(sf::Vector2f(900, 600));  // 900x600 size
            blackRect.setFillColor(sf::Color::Black);  // Black color
            blackRect.setPosition(50, 800 - 20 - 600);  // Same position as before, but inside the white border

            // Draw both the white border and black rectangle
            window.draw(whiteBorder);
            window.draw(blackRect);
        }

        // Display the contents of the window
        window.display();
    }

    return 0;
}
