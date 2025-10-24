#include "entity.h"
#include "neuralnetwork.h"
#include "gui.h"
#include <iostream>

Entity::Entity(float x, float y, float r, sf::Color c, int in, int hid, int out)
    : pos(x, y), vel(0, 0), radius(r), color(c),
    brain(std::make_unique<NeuralNetwork>(in, hid, out)),
    energy(100), fitness(0), generation(1), age(0), timeSinceLastMeal(1) {}

void Entity::update(float dt, float width, float height, const std::vector<TerrainTile>& terrain) {
    // Vieillissement
    age++;
    timeSinceLastMeal += dt;
    vel *= 0.95f;  // Réduit de 5% par frame

    // Si trop lent, arrêt complet
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (currentSpeed < 0.5f) {
        vel.x = 0;
        vel.y = 0;
    }

    // Application du mouvement
    sf::Vector2f oldPos = pos;
    pos += vel * dt;

    // Wrap around edges
    if (pos.x <= 0) {
        pos.x = width - 4.0f;
    } else if (pos.x >= width) {
        pos.x = 4.0f;
    }

    if (pos.y <= 0) {
        pos.y = height - 4.0f;
    } else if (pos.y >= height) {
        pos.y = 4.0f;
    }

    // Collision avec obstacles
    for (const auto& tile : terrain) {
        if (tile.collidesWith(pos, radius)) {
            pos = oldPos;
            vel *= -0.5f;
            break;
        }
    }

    // Effets du terrain
    for (const auto& tile : terrain) {
        sf::FloatRect tileBounds = tile.shape.getGlobalBounds();
        if (tileBounds.contains(pos)) {
            switch(tile.type) {
            case TerrainType::WATER:
                vel *= 0.7f;
                energy -= 0.02f * dt;
                break;
            case TerrainType::DESERT:
                energy -= 0.03f * dt;
                break;
            default:
                break;
            }
            break;
        }
    }
    energy -= 0.01f * dt;
}

void Entity::draw(sf::RenderWindow& window, bool showDirection) const {
    sf::CircleShape shape(radius);
    shape.setPosition(pos - sf::Vector2f(radius, radius));
    shape.setFillColor(color);
    window.draw(shape);

    if (showDirection) {
        const std::array<sf::Vertex,2> line{{
            {pos, sf::Color::White},
            {pos + vel * 2.0f, sf::Color::White}
        }};
        window.draw(line.data(), 2, sf::PrimitiveType::Lines);
    }
}

float Entity::distanceTo(const Entity& other) const {
    const sf::Vector2f diff = pos - other.pos;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

float Entity::distanceTo(const sf::Vector2f& point) const {
    const sf::Vector2f diff = pos - point;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

bool Entity::isDead() const {
    return energy <= 0 || age > 5400; // ~90 secondes à 60 FPS = 3 générations
}



//==========PROIE==============
Prey::Prey(float x, float y) : Entity(x, y, 5, sf::Color::Green, 8, 20, 2) {}

void Prey::think(const std::vector<std::unique_ptr<Predator>>& predators,
                 const std::vector<std::unique_ptr<Food>>& foods) {
    float closestPredDist = 1e6f;
    sf::Vector2f closestPred(GUI::res_width/2, GUI::res_height/2);

    float closestFoodDist = 1e6f;
    sf::Vector2f closestFood(GUI::res_width/2, GUI::res_height/2);

    for (const auto& pred : predators) {
        float d = distanceTo(*pred);
        if (d < closestPredDist) {
            closestPredDist = d;
            closestPred = pred->pos;
        }
    }

    for (const auto& food : foods) {
        if (!food->consumed) {
            float d = distanceTo(food->pos);
            if (d < closestFoodDist) {
                closestFoodDist = d;
                closestFood = food->pos;
            }
        }
    }

    // Fitness basé sur distance et survie
    if (closestPredDist < DETECTION_RADIUS) {
        fitness -= 0.5f;
    } else {
        fitness += 0.08f;
    }

    if (closestFoodDist < 50.0f) {
        fitness += 0.2f; // Récompense pour être près de nourriture
    }



    const sf::Vector2f toPred = closestPred - pos;
    const sf::Vector2f toFood = closestFood - pos;

    const std::array<float, 8> inputs = {
        toPred.x / GUI::res_width,
        toPred.y / GUI::res_height,
        closestPredDist / 500.0f,
        toFood.x / GUI::res_width,
        toFood.y / GUI::res_height,
        closestFoodDist / 500.0f,
        energy / 100.0f,
        timeSinceLastMeal / 10.0f
    };

    const auto outputs = brain->forward(inputs);

    const float angle = (outputs[0] - 0.5f) * 2.0f * 3.14159f;

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }
    float speed = speedOutput * 100.0f;

    vel.x = std::cos(angle) * speed;
    vel.y = std::sin(angle) * speed;

    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    if (currentSpeed < 10){
        fitness -= 0.1f;
    }
}
//==========PREDATEUR===========
Predator::Predator(float x, float y) : Entity(x, y, 8, sf::Color::Red, 8, 20, 2), kills(0) {}

bool Predator::isStarving() const {             // check si il est sur le point de mourir de faim
    return timeSinceLastMeal > STARVATION_TIME;
}

bool Predator::isHungry() const {                   //check si il a faim (valeur arbitraire)
    return timeSinceLastMeal +10 > STARVATION_TIME;
}

void Predator::think(const std::vector<std::unique_ptr<Prey>>& preys) {
    float closestDist = 1e6f;
    sf::Vector2f closestPrey(GUI::res_width/2, GUI::res_height/2);

    for (const auto& prey : preys) {
        float d = distanceTo(*prey);
        if (d < closestDist) {
            closestDist = d;
            closestPrey = prey->pos;
        }
    }
    // Fitness basé sur proximité et chasse
    if (closestDist < DETECTION_RADIUS) {
        fitness += 0.001f;
    } else {
        fitness -= 1.0f;
    }

    // reaction basé sur la proximité/faim
    if (closestDist < HUNGER_RADIUS and isHungry()){
        //chasse direct
        sf::Vector2f chaseDir = closestPrey - pos;
        float magnitude = std::sqrt(chaseDir.x * chaseDir.x + chaseDir.y * chaseDir.y);
        chaseDir /= magnitude;
        vel = chaseDir * 180.0f; // Poursuite rapide
    }

    if (isStarving()) {
        fitness -= fitness; // Forte pénalité pour la faim
    }



    const sf::Vector2f toPrey = closestPrey - pos;
    const std::array<float, 8> inputs = {
        toPrey.x / GUI::res_width,
        toPrey.y / GUI::res_height,
        closestDist / 500.0f,
        vel.x / 200.0f,
        vel.y / 200.0f,
        energy / 100.0f,
        timeSinceLastMeal / 20.0f,
        (float)kills / 10.0f
    };

    const auto outputs = brain->forward(inputs);

    const float angle = (outputs[0] - 0.5f) * 2.0f * 3.14159f;

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }
    float speed = speedOutput * 150.0f;

    vel.x = std::cos(angle) * speed;
    vel.y = std::sin(angle) * speed;

    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    if (currentSpeed < 10){
        fitness -= 0.1f;
    }
}
