#include "terraintype.h"
#include <math.h>

// Constructeur
TerrainTile::TerrainTile(TerrainType t) : type(t) {}

// Définition des méthodes
void TerrainTile::setAsPolygon(const std::vector<sf::Vector2f>& points, sf::Color color) {
    shape.setPointCount(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        shape.setPoint(i, points[i]);
    }
    shape.setFillColor(color);
}

void TerrainTile::addRock(const std::vector<sf::Vector2f>& points, sf::Color color) {
    sf::ConvexShape rock;
    rock.setPointCount(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        rock.setPoint(i, points[i]);
    }
    rock.setFillColor(color);
    obstacles.push_back(rock);
}

bool TerrainTile::collidesWith(sf::Vector2f pos, float radius) const {
    for (const auto& obs : obstacles) {
        sf::FloatRect bounds = obs.getGlobalBounds();
        sf::Vector2f center(bounds.position.x + bounds.size.x/2, bounds.position.y + bounds.size.y/2);
        sf::Vector2f diff = pos - center;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        float obstacleRadius = std::max(bounds.size.x, bounds.size.y) / 2;
        if (dist < (radius + obstacleRadius)) return true;
    }
    return false;
}

void TerrainTile::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    for (const auto& obs : obstacles) {
        window.draw(obs);
    }
}
