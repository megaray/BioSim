#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include "gui.h"
#include "simulation.h"


// ============ MAIN ============
int main() {
    GUI::GUIControls gui;
    sf::RenderWindow window(sf::VideoMode({GUI::res_width, GUI::res_height}), "Simulation IA Ecosystem - Proies vs Predateurs");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Font not found. Using default rendering.\n";
    }

    Simulation sim;
    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                sim.handleKeyPress(keyPressed->code);
            }
        }

        float dt = clock.restart().asSeconds();
        dt *= gui.fastForwardRate;
        sim.update(dt);

        window.clear(sf::Color(20, 20, 30));
        sim.draw(window, font);
        window.display();
    }

    return 0;
}
