#include "survivallogic.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
//=========LA BOUFFE==========
sf::Vector2f pos;
float energy;
bool consumed;
Food::Food(float x, float y, float e) : pos(x, y), energy(e), consumed(false) {}

void Food::draw(sf::RenderWindow& window) const {
    if (consumed) return;
    sf::CircleShape shape(3);
    shape.setPosition(pos - sf::Vector2f(3, 3));
    shape.setFillColor(sf::Color(150, 255, 150));
    window.draw(shape);
}
//==============================
