#include "entity.h"
#include "neuralnetwork.h"
#include "gui.h"
#include <iostream>

Entity::Entity(float x, float y, float r, sf::Color c, int in, int hid, int out)
    : pos(x, y), vel(0, 0), acc(0, 0), radius(r), color(c),
    brain(std::make_unique<NeuralNetwork>(in, hid, out)),
    energy(100), fitness(0), generation(1), age(0), timeSinceLastMeal(1), maxSpeed(200.0f) {}

void Entity::update(float dt, float width, float height, const std::vector<TerrainTile>& terrain) {
    // Vieillissement
    age++;
    timeSinceLastMeal += dt;

    // PHYSICS-BASED MOVEMENT:
    // 1. Apply acceleration to velocity
    vel += acc * dt;

    // 2. Apply friction (realistic drag)
    vel *= 0.98f;  // Slight friction every frame

    // 3. Clamp to max speed
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (currentSpeed > maxSpeed) {
        vel *= (maxSpeed / currentSpeed);
    }

    // 4. Stop if too slow (prevent drift)
    if (currentSpeed < 0.5f) {
        vel.x = 0;
        vel.y = 0;
    }

    // 5. Reset acceleration for next frame
    acc = sf::Vector2f(0, 0);

    // 6. Apply velocity to position
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
            vel *= -0.3f;  // Bounce back (energy loss)
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
<<<<<<< HEAD

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }
    float speed = speedOutput * 100.0f;
=======
>>>>>>> 05cc87d (modif complet)

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }

<<<<<<< HEAD
=======
    // ACCELERATION-BASED: Apply force instead of setting velocity directly
    float forceStrength = speedOutput * 300.0f;  // Acceleration force
    acc.x = std::cos(angle) * forceStrength;
    acc.y = std::sin(angle) * forceStrength;

>>>>>>> 05cc87d (modif complet)
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    if (currentSpeed < 10){
        fitness -= 0.1f;
    }
}
//==========PREDATEUR===========
Predator::Predator(float x, float y) : Entity(x, y, 8, sf::Color::Red, 8, 20, 2), kills(0) {
    maxSpeed = 250.0f;  // Predators are faster
}

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

    // ACCELERATION-BASED: Apply strong chase force when hungry and close
    if (closestDist < HUNGER_RADIUS && isHungry()) {
        sf::Vector2f chaseDir = closestPrey - pos;
        float magnitude = std::sqrt(chaseDir.x * chaseDir.x + chaseDir.y * chaseDir.y);
        if (magnitude > 0) {
            chaseDir /= magnitude;  // Normalize
            // Apply strong acceleration towards prey
            acc += chaseDir * 500.0f;  // Strong chase force
        }
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
<<<<<<< HEAD

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }
    float speed = speedOutput * 150.0f;
=======

    float speedOutput = outputs[1];
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }

    // ACCELERATION-BASED: Neural network adds to acceleration
    float forceStrength = speedOutput * 400.0f;  // Predators have stronger acceleration
    acc.x += std::cos(angle) * forceStrength;
    acc.y += std::sin(angle) * forceStrength;
>>>>>>> 05cc87d (modif complet)

    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

<<<<<<< HEAD
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

=======
>>>>>>> 05cc87d (modif complet)
    if (currentSpeed < 10){
        fitness -= 0.1f;
    }
}
