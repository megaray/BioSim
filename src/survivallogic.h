#ifndef SURVIVALLOGIC_H
#define SURVIVALLOGIC_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class Food {
public:
    sf::Vector2f pos;
    float energy;
    bool consumed;

    Food(float x, float y, float e = 50.0f);
    void draw(sf::RenderWindow& window) const;
};
#endif // SURVIVALLOGIC_H
