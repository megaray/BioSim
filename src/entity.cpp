#include "entity.h"
#include "neuralnetwork.h"
#include "gui.h"
#include <iostream>

// ============================================================================
// CONSTRUCTEUR ENTITY
// ============================================================================
Entity::Entity(float x, float y, float r, sf::Color c, int in, int hid, int out)
    : pos(x, y), vel(0, 0), acc(0, 0), radius(r), color(c),
    brain(std::make_unique<NeuralNetwork>(in, hid, out)),
    energy(100), fitness(0), generation(1), age(0), timeSinceLastMeal(1), maxSpeed(200.0f) {}

// ============================================================================
// UPDATE - GESTION PHYSIQUE DU MOUVEMENT
// ============================================================================
// Cette fonction implémente un système physique basé sur l'accélération.
// Le cerveau neuronal contrôle l'ACCÉLÉRATION, pas directement la vitesse.
// Cela crée un mouvement plus naturel et réaliste.
// ============================================================================
void Entity::update(float dt, float width, float height, const std::vector<TerrainTile>& terrain) {
    // ========== VIEILLISSEMENT ==========
    // Incrémenter l'âge et le temps depuis le dernier repas
    age++;
    timeSinceLastMeal += dt;

    // ========== ÉTAPE 1: APPLIQUER L'ACCÉLÉRATION À LA VITESSE ==========
    // L'accélération (acc) a été définie par think() via le réseau neuronal
    // On l'applique maintenant à la vitesse pour modifier le mouvement
    // Formule physique: velocity += acceleration * deltaTime
    vel += acc * dt;

    // ========== ÉTAPE 2: APPLIQUER LA FRICTION ==========
    // La friction simule la résistance de l'air/eau/terrain
    // Chaque frame, on réduit la vitesse de 2% (multiplication par 0.98)
    // Sans friction, les entités accéléreraient infiniment
    vel *= 0.98f;

    // ========== ÉTAPE 3: LIMITER LA VITESSE MAXIMALE ==========
    // Calculer la magnitude (norme) de la vitesse actuelle
    // Formule: speed = sqrt(vx² + vy²)
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    // Si la vitesse dépasse maxSpeed, on la ramène à maxSpeed
    // en normalisant le vecteur vitesse et en le multipliant par maxSpeed
    if (currentSpeed > maxSpeed) {
        // Normalisation: diviser par la magnitude actuelle
        // Puis multiplier par la magnitude désirée (maxSpeed)
        vel *= (maxSpeed / currentSpeed);
    }

    // ========== ÉTAPE 4: SEUIL DE VITESSE MINIMALE ==========
    // Si la vitesse est très faible (< 1.0), on considère l'entité immobile
    // Cela évite les micro-mouvements et économise les calculs
    // IMPORTANT: Ne PAS mettre ce seuil trop haut, sinon les entités
    // ne peuvent jamais démarrer depuis l'arrêt
    if (currentSpeed < 1.0f) {
        vel.x = 0;
        vel.y = 0;
    }

    // ========== ÉTAPE 5: RÉINITIALISER L'ACCÉLÉRATION ==========
    // L'accélération est une "force" qui doit être réappliquée chaque frame
    // On la remet à zéro ici, et think() la redéfinira à la prochaine frame
    acc = sf::Vector2f(0, 0);

    // ========== ÉTAPE 6: APPLIQUER LA VITESSE À LA POSITION ==========
    // Sauvegarder l'ancienne position (pour détection de collision)
    sf::Vector2f oldPos = pos;

    // Déplacer l'entité selon sa vitesse
    // Formule physique: position += velocity * deltaTime
    pos += vel * dt;

    // ========== GESTION DES BORDS DE LA CARTE (WRAP-AROUND) ==========
    // Si l'entité sort par un bord, elle réapparaît de l'autre côté
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

    // ========== COLLISION AVEC LES OBSTACLES ==========
    // Vérifier si l'entité entre en collision avec le terrain
    for (const auto& tile : terrain) {
        if (tile.collidesWith(pos, radius)) {
            // En cas de collision:
            // 1. Remettre l'entité à son ancienne position
            pos = oldPos;
            // 2. Inverser la vitesse avec perte d'énergie (rebond à 30%)
            vel *= -0.3f;
            break;
        }
    }

    // ========== EFFETS DU TERRAIN SUR LE MOUVEMENT ==========
    // Différents types de terrain affectent le mouvement et l'énergie
    for (const auto& tile : terrain) {
        sf::FloatRect tileBounds = tile.shape.getGlobalBounds();
        if (tileBounds.contains(pos)) {
            switch(tile.type) {
            case TerrainType::WATER:
                // L'eau ralentit fortement (30% de réduction de vitesse)
                vel *= 0.7f;
                // Coût énergétique pour nager
                energy -= 0.02f * dt;
                break;
            case TerrainType::DESERT:
                // Le désert épuise l'énergie (chaleur)
                energy -= 0.03f * dt;
                break;
            default:
                break;
            }
            break;
        }
    }

    // Coût énergétique de base (métabolisme)
    energy -= 0.01f * dt;
}

// ============================================================================
// DRAW - AFFICHAGE DE L'ENTITÉ
// ============================================================================
void Entity::draw(sf::RenderWindow& window, bool showDirection) const {
    // Dessiner le cercle représentant l'entité
    sf::CircleShape shape(radius);
    shape.setPosition(pos - sf::Vector2f(radius, radius));
    shape.setFillColor(color);
    window.draw(shape);

    // Si demandé, dessiner une ligne indiquant la direction du mouvement
    if (showDirection) {
        const std::array<sf::Vertex,2> line{{
            {pos, sf::Color::White},
            {pos + vel * 2.0f, sf::Color::White}
        }};
        window.draw(line.data(), 2, sf::PrimitiveType::Lines);
    }
}

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================
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



// ============================================================================
// ============================================================================
//                              PROIE (PREY)
// ============================================================================
// ============================================================================

Prey::Prey(float x, float y) : Entity(x, y, 5, sf::Color::Green, 8, 20, 2) {}

// ============================================================================
// THINK - LOGIQUE DE DÉCISION DE LA PROIE
// ============================================================================
// Cette fonction est appelée à chaque frame pour décider du comportement
// de la proie en fonction de son environnement.
// ============================================================================
void Prey::think(const std::vector<std::unique_ptr<Predator>>& predators,
                 const std::vector<std::unique_ptr<Food>>& foods) {
    // ========== DÉTECTION DU PRÉDATEUR LE PLUS PROCHE ==========
    float closestPredDist = 1e6f;
    sf::Vector2f closestPred(GUI::res_width/2, GUI::res_height/2);

    for (const auto& pred : predators) {
        float d = distanceTo(*pred);
        if (d < closestPredDist) {
            closestPredDist = d;
            closestPred = pred->pos;
        }
    }

    // ========== DÉTECTION DE LA NOURRITURE LA PLUS PROCHE ==========
    float closestFoodDist = 1e6f;
    sf::Vector2f closestFood(GUI::res_width/2, GUI::res_height/2);

    for (const auto& food : foods) {
        if (!food->consumed) {
            float d = distanceTo(food->pos);
            if (d < closestFoodDist) {
                closestFoodDist = d;
                closestFood = food->pos;
            }
        }
    }

    // ========== CALCUL DU FITNESS (RÉCOMPENSES/PÉNALITÉS) ==========
    // Le fitness guide l'évolution: les proies avec meilleur fitness survivent

    // DANGER: Pénalité si un prédateur est proche
    if (closestPredDist < DETECTION_RADIUS) {
        fitness -= 0.5f;
    } else {
        // SÉCURITÉ: Récompense si loin des prédateurs
        fitness += 0.08f;
    }

    // NOURRITURE: Récompense si proche de la nourriture
    if (closestFoodDist < 50.0f) {
        fitness += 0.2f;
    }

    // ========== PRÉPARATION DES INPUTS DU RÉSEAU NEURONAL ==========
    // Calculer les vecteurs de direction
    const sf::Vector2f toPred = closestPred - pos;
    const sf::Vector2f toFood = closestFood - pos;

    // Normaliser les inputs entre 0 et 1 pour le réseau neuronal
    const std::array<float, 8> inputs = {
        toPred.x / GUI::res_width,           // Direction X vers prédateur (normalisée)
        toPred.y / GUI::res_height,          // Direction Y vers prédateur (normalisée)
        closestPredDist / 500.0f,            // Distance au prédateur (normalisée)
        toFood.x / GUI::res_width,           // Direction X vers nourriture (normalisée)
        toFood.y / GUI::res_height,          // Direction Y vers nourriture (normalisée)
        closestFoodDist / 500.0f,            // Distance à la nourriture (normalisée)
        energy / 100.0f,                     // Niveau d'énergie (normalisée)
        timeSinceLastMeal / 10.0f            // Temps depuis dernier repas (normalisé)
    };

    // ========== DÉCISION DU RÉSEAU NEURONAL ==========
    // Le réseau prend les 8 inputs et produit 2 outputs:
    // - outputs[0] : angle de déplacement (0 à 1, converti en 0 à 2π)
    // - outputs[1] : intensité de l'accélération (0 à 1)
    const auto outputs = brain->forward(inputs);

    // ========== CALCUL DE L'ANGLE DE DÉPLACEMENT ==========
    // Convertir output[0] (0 à 1) en angle en radians (0 à 2π)
    // Formule: angle = (output - 0.5) * 2 * 2π
    // Cela donne un angle de -π à +π (ou 0 à 2π)
    const float angle = (outputs[0] - 0.5f) * 2.0f * 3.14159f;

    // ========== CALCUL DE LA FORCE D'ACCÉLÉRATION ==========
    // Output[1] contient la vitesse désirée (entre 0 et 1)
    float speedOutput = outputs[1];

    // Zone morte: Si output < 0.3, on considère que c'est "arrêt"
    // Cela évite les micro-mouvements parasites
    // On remape 0.3-1.0 vers 0.0-1.0 pour avoir toute la plage utilisable
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        // Remapper [0.3, 1.0] vers [0.0, 1.0]
        // Formule: (valeur - min) / (max - min)
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }

    // ========== APPLICATION DE L'ACCÉLÉRATION ==========
    // Convertir la vitesse désirée en force d'accélération
    // Plus la valeur est grande, plus l'accélération est forte
    // 300.0f est un facteur d'échelle pour avoir des accélérations raisonnables
    float forceStrength = speedOutput * 300.0f;

    // Décomposer la force en composantes X et Y selon l'angle
    // cos(angle) donne la composante X
    // sin(angle) donne la composante Y
    acc.x = std::cos(angle) * forceStrength;
    acc.y = std::sin(angle) * forceStrength;

    // ========== CALCUL DE LA VITESSE ACTUELLE ==========
    // Calculer la magnitude de la vitesse pour le fitness
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    // PÉNALITÉ POUR IMMOBILITÉ: Les proies qui ne bougent pas assez
    // sont pénalisées (cela encourage l'exploration)
    // Note: Seuil à 5 pour éviter de bloquer les entités qui démarrent
    if (currentSpeed < 5.0f){
        fitness -= 0.05f;  // Pénalité réduite
    }
}

// ============================================================================
// ============================================================================
//                           PRÉDATEUR (PREDATOR)
// ============================================================================
// ============================================================================

Predator::Predator(float x, float y) : Entity(x, y, 8, sf::Color::Red, 8, 20, 2), kills(0) {
    maxSpeed = 250.0f;  // Les prédateurs sont plus rapides que les proies
}

bool Predator::isStarving() const {
    return timeSinceLastMeal > STARVATION_TIME;
}

bool Predator::isHungry() const {
    return timeSinceLastMeal + 10 > STARVATION_TIME;
}

// ============================================================================
// THINK - LOGIQUE DE DÉCISION DU PRÉDATEUR
// ============================================================================
void Predator::think(const std::vector<std::unique_ptr<Prey>>& preys) {
    // ========== DÉTECTION DE LA PROIE LA PLUS PROCHE ==========
    float closestDist = 1e6f;
    sf::Vector2f closestPrey(GUI::res_width/2, GUI::res_height/2);

    for (const auto& prey : preys) {
        float d = distanceTo(*prey);
        if (d < closestDist) {
            closestDist = d;
            closestPrey = prey->pos;
        }
    }

    // ========== CALCUL DU FITNESS ==========
    // PROCHE DE PROIE: Récompense si proche d'une proie
    if (closestDist < DETECTION_RADIUS) {
        fitness += 0.001f;
    } else {
        // LOIN DE PROIE: Pénalité si loin (encourage la chasse)
        fitness -= 1.0f;
    }

    // ========== COMPORTEMENT DE CHASSE INSTINCTIF ==========
    // Si une proie est proche ET le prédateur a faim,
    // appliquer une forte accélération vers la proie (instinct de chasse)
    if (closestDist < HUNGER_RADIUS && isHungry()) {
        sf::Vector2f chaseDir = closestPrey - pos;
        float magnitude = std::sqrt(chaseDir.x * chaseDir.x + chaseDir.y * chaseDir.y);

        if (magnitude > 0) {
            // Normaliser le vecteur direction
            chaseDir /= magnitude;
            // Appliquer une forte accélération vers la proie
            acc += chaseDir * 500.0f;  // Force de chasse puissante
        }
    }

    // FAIM CRITIQUE: Forte pénalité si affamé (force l'évolution à chasser)
    if (isStarving()) {
        fitness -= fitness; // Annule tout le fitness accumulé
    }

    // ========== PRÉPARATION DES INPUTS DU RÉSEAU NEURONAL ==========
    const sf::Vector2f toPrey = closestPrey - pos;
    const std::array<float, 8> inputs = {
        toPrey.x / GUI::res_width,           // Direction X vers proie
        toPrey.y / GUI::res_height,          // Direction Y vers proie
        closestDist / 500.0f,                // Distance à la proie
        vel.x / 200.0f,                      // Vitesse actuelle X
        vel.y / 200.0f,                      // Vitesse actuelle Y
        energy / 100.0f,                     // Niveau d'énergie
        timeSinceLastMeal / 20.0f,           // Temps depuis dernier repas
        (float)kills / 10.0f                 // Nombre de captures
    };

    // ========== DÉCISION DU RÉSEAU NEURONAL ==========
    const auto outputs = brain->forward(inputs);

    // ========== CALCUL DE L'ANGLE DE DÉPLACEMENT ==========
    const float angle = (outputs[0] - 0.5f) * 2.0f * 3.14159f;

    // ========== CALCUL DE LA FORCE D'ACCÉLÉRATION ==========
    float speedOutput = outputs[1];

    // Zone morte pour éviter les micro-mouvements
    if (speedOutput < 0.3f) {
        speedOutput = 0.0f;
    } else {
        speedOutput = (speedOutput - 0.3f) / 0.7f;
    }

    // ========== APPLICATION DE L'ACCÉLÉRATION ==========
    // Les prédateurs ont une accélération plus forte (400 vs 300)
    // IMPORTANT: Utiliser += pour ajouter à l'accélération instinctive
    float forceStrength = speedOutput * 400.0f;
    acc.x += std::cos(angle) * forceStrength;
    acc.y += std::sin(angle) * forceStrength;

    // ========== CALCUL DE LA VITESSE ACTUELLE ==========
    float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    // PÉNALITÉ POUR IMMOBILITÉ
    if (currentSpeed < 5.0f){
        fitness -= 0.05f;  // Pénalité réduite
    }
}
