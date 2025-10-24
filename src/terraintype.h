#ifndef TERRAINTIPE_H
#define TERRAINTIPE_H

#include <SFML/Graphics.hpp>
#include <vector>
// ============ TYPES DE TERRAIN ============
enum class TerrainType {
    NORMAL,
    WATER,
    GRASS,
    DESERT
};

// ============ STRUCTURE DE TUILE ============
struct TerrainTile {
    TerrainType type;
    sf::ConvexShape shape;
    std::vector<sf::ConvexShape> obstacles;

    // Constructeur
    explicit TerrainTile(TerrainType t);

    // Méthodes
    void setAsPolygon(const std::vector<sf::Vector2f>& points, sf::Color color);
    void addRock(const std::vector<sf::Vector2f>& points, sf::Color color);
    bool collidesWith(sf::Vector2f pos, float radius) const;
    void draw(sf::RenderWindow& window) const;
};

#endif // TERRAINTIPE_H
