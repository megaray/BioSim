#ifndef ENTITY_H
#define ENTITY_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <memory>
#include "neuralnetwork.h"
#include "terraintype.h"
#include "survivallogic.h"

class Prey;
class Predator;
//===========ENTITE DE BASE==========
class Entity {
public:
    sf::Vector2f pos, vel;
    float radius;
    sf::Color color;
    std::unique_ptr<NeuralNetwork> brain;
    float energy;
    float fitness;
    int generation;
    int age;
    float timeSinceLastMeal;

    Entity(float x, float y, float r, sf::Color c, int in, int hid, int out);

    virtual ~Entity() = default;

    void update(float dt, float width, float height, const std::vector<TerrainTile>& terrain);

    void draw(sf::RenderWindow& window, bool showDirection) const;

    float distanceTo(const Entity& other) const;

    float distanceTo(const sf::Vector2f& point) const;

    bool isDead() const;
};
//============PROIE=============
class Prey : public Entity {
public:
    static constexpr float DETECTION_RADIUS = 80.0f;

    Prey(float x, float y);

    void think(const std::vector<std::unique_ptr<Predator>>& predators, const std::vector<std::unique_ptr<Food>>& foods);
};

// ============ PRÉDATEUR ============
class Predator : public Entity {
public:
    static constexpr float DETECTION_RADIUS = 100.0f;
    static constexpr float HUNGER_RADIUS = 250.0f;
    static constexpr float STARVATION_TIME = 20.0f;
    int kills;
    Predator(float x, float y);
    void think(const std::vector<std::unique_ptr<Prey>>& preys);
    bool isStarving() const;
    bool isHungry() const;
};

#endif // ENTITY_H
