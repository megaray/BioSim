#include "simulation.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <memory>
#include <array>
#include <deque>


//PRIVATE DEF
std::mt19937& Simulation::getRNG() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

float Simulation::randFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(getRNG());
}

int Simulation::randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getRNG());
}

void Simulation::generateTerrain() {
    terrain.clear();

    // Générer des lacs (eau en forme organique)
    int numLakes = randInt(2, 4);
    for (int i = 0; i < numLakes; ++i) {
        float centerX = randFloat(100, GUI::res_width - 100);
        float centerY = randFloat(100, GUI::res_height - 100);
        float baseRadius = randFloat(50, 80);

        std::vector<sf::Vector2f> lakePoints;
        int numPoints = randInt(8, 12);
        for (int j = 0; j < numPoints; ++j) {
            float angle = (j * 2.0f * 3.14159f) / numPoints;
            float radius = baseRadius + randFloat(-20, 20);
            lakePoints.push_back({
                centerX + std::cos(angle) * radius,
                centerY + std::sin(angle) * radius
            });
        }

        TerrainTile lake(TerrainType::WATER);
        lake.setAsPolygon(lakePoints, sf::Color(50, 100, 200, 120));
        terrain.push_back(lake);
    }

    // Générer des rivières
    int numRivers = randInt(1, 2);
    for (int i = 0; i < numRivers; ++i) {
        //float startX = (i == 0) ? 0 : res_width;
        float y = randFloat(100, GUI::res_height - 100);
        float width = randFloat(30, 50);

        std::vector<sf::Vector2f> riverPoints;
        float segments = 10;
        for (int j = 0; j <= segments; ++j) {
            float t = j / segments;
            float x = (i == 0) ? (t * GUI::res_width) : (GUI::res_width - t * GUI::res_width);
            float yOffset = std::sin(t * 6.0f) * 40.0f;

            riverPoints.push_back({x, y + yOffset - width/2});
        }
        for (int j = segments; j >= 0; --j) {
            float t = j / segments;
            float x = (i == 0) ? (t * GUI::res_width) : (GUI::res_width - t * GUI::res_width);
            float yOffset = std::sin(t * 6.0f) * 40.0f;

            riverPoints.push_back({x, y + yOffset + width/2});
        }

        TerrainTile river(TerrainType::WATER);
        river.setAsPolygon(riverPoints, sf::Color(50, 100, 200, 120));
        terrain.push_back(river);
    }

    // Générer des prairies (patches irréguliers)
    int numGrass = randInt(6, 10);
    for (int i = 0; i < numGrass; ++i) {
        float centerX = randFloat(50, GUI::res_width - 50);
        float centerY = randFloat(50, GUI::res_height - 50);
        float baseRadius = randFloat(60, 100);

        std::vector<sf::Vector2f> grassPoints;
        int numPoints = randInt(6, 10);
        for (int j = 0; j < numPoints; ++j) {
            float angle = (j * 2.0f * 3.14159f) / numPoints;
            float radius = baseRadius + randFloat(-30, 30);
            grassPoints.push_back({
                centerX + std::cos(angle) * radius,
                centerY + std::sin(angle) * radius
            });
        }

        TerrainTile grass(TerrainType::GRASS);
        grass.setAsPolygon(grassPoints, sf::Color(100, 200, 100, 100));

        // Ajouter quelques arbres dans les prairies
        int numTrees = randInt(2, 5);
        for (int t = 0; t < numTrees; ++t) {
            float treeX = centerX + randFloat(-baseRadius/2, baseRadius/2);
            float treeY = centerY + randFloat(-baseRadius/2, baseRadius/2);
            float treeRadius = randFloat(8, 15);

            std::vector<sf::Vector2f> treePoints;
            int treePointCount = randInt(6, 8);
            for (int p = 0; p < treePointCount; ++p) {
                float angle = (p * 2.0f * 3.14159f) / treePointCount;
                float r = treeRadius + randFloat(-3, 3);
                treePoints.push_back({
                    treeX + std::cos(angle) * r,
                    treeY + std::sin(angle) * r
                });
            }
            grass.addRock(treePoints, sf::Color(60, 120, 60));
        }

        terrain.push_back(grass);
    }

    // Générer des déserts (zones arides)
    int numDeserts = randInt(3, 5);
    for (int i = 0; i < numDeserts; ++i) {
        float centerX = randFloat(50, GUI::res_width - 50);
        float centerY = randFloat(50, GUI::res_height - 50);
        float baseRadius = randFloat(70, 120);

        std::vector<sf::Vector2f> desertPoints;
        int numPoints = randInt(5, 8);
        for (int j = 0; j < numPoints; ++j) {
            float angle = (j * 2.0f * 3.14159f) / numPoints;
            float radius = baseRadius + randFloat(-25, 25);
            desertPoints.push_back({
                centerX + std::cos(angle) * radius,
                centerY + std::sin(angle) * radius
            });
        }

        TerrainTile desert(TerrainType::DESERT);
        desert.setAsPolygon(desertPoints, sf::Color(220, 200, 100, 100));

        // Ajouter des rochers anguleux dans le désert
        int numRocks = randInt(3, 7);
        for (int r = 0; r < numRocks; ++r) {
            float rockX = centerX + randFloat(-baseRadius/2, baseRadius/2);
            float rockY = centerY + randFloat(-baseRadius/2, baseRadius/2);
            float rockSize = randFloat(10, 20);

            std::vector<sf::Vector2f> rockPoints;
            int rockPointCount = randInt(5, 7);
            for (int p = 0; p < rockPointCount; ++p) {
                float angle = (p * 2.0f * 3.14159f) / rockPointCount;
                float r = rockSize + randFloat(-5, 5);
                rockPoints.push_back({
                    rockX + std::cos(angle) * r,
                    rockY + std::sin(angle) * r
                });
            }
            desert.addRock(rockPoints, sf::Color(100, 90, 80));
        }

        terrain.push_back(desert);
    }
}

void Simulation::spawnFood() {
    // Spawn dans les prairies
    for (const auto& tile : terrain) {
        if (tile.type == TerrainType::GRASS && randInt(0, 100) < 40) {
            sf::FloatRect grassBounds = tile.shape.getGlobalBounds();
            float fx = grassBounds.position.x + randFloat(20, grassBounds.size.x - 20);
            float fy = grassBounds.position.y + randFloat(20, grassBounds.size.y - 20);

            // Éviter de spawner sur les obstacles
            bool onObstacle = false;
            for (const auto& obs : tile.obstacles) {
                sf::FloatRect obsBounds = obs.getGlobalBounds();
                if (obsBounds.contains({fx, fy})) {
                    onObstacle = true;
                    break;
                }
            }

            if (!onObstacle) {
                foods.push_back(std::make_unique<Food>(fx, fy));
            }
        }
    }
}

//PUBLIC DEF
Simulation::Simulation() : generation(1), timer(0), preyGeneration(1), predGeneration(1),
    graphUpdateTimer(0), foodSpawnTimer(0) {
    generateTerrain();

    preys.reserve(25);
    for (int i = 0; i < 25; ++i) {
        preys.push_back(std::make_unique<Prey>(randFloat(50, GUI::res_width-50), randFloat(50, GUI::res_height-50)));
    }
    predators.reserve(6);
    for (int i = 0; i < 6; ++i) {
        predators.push_back(std::make_unique<Predator>(randFloat(50, GUI::res_width-50), randFloat(50, GUI::res_height-50)));
    }

    spawnFood();
}

void Simulation::update(float dt) {
    //std::cout<<dt<<std::endl;
    timer += dt;
    graphUpdateTimer += dt;
    foodSpawnTimer += dt;

    // Spawn nourriture périodique
    if (foodSpawnTimer > 5.0f) {
        spawnFood();
        foodSpawnTimer = 0;
    }

    // Update proies
    for (auto& prey : preys) {
        prey->think(predators, foods);
        prey->update(dt, GUI::res_width, GUI::res_height, terrain);

        // Manger nourriture
        for (auto& food : foods) {
            if (!food->consumed && prey->distanceTo(food->pos) < 10.0f) {
                prey->energy += food->energy;
                prey->fitness += 30.0f;
                prey->timeSinceLastMeal = 0;
                food->consumed = true;
            }
        }
    }

    // Update prédateurs
    for (auto& pred : predators) {
        pred->think(preys);
        pred->update(dt, GUI::res_width, GUI::res_height, terrain);
    }

    // Captures
    for (auto& pred : predators) {
        preys.erase(
            std::remove_if(preys.begin(), preys.end(),
                           [&pred](const auto& prey) {
                               if (pred->distanceTo(*prey) < pred->radius + prey->radius) {
                                   pred->energy += 80;
                                   pred->fitness += 150;
                                   pred->kills++;
                                   pred->timeSinceLastMeal = 0;
                                   return true;
                               }
                               return false;
                           }),
            preys.end()
            );
    }

    // Mort par faim/vieillesse
    predators.erase(
        std::remove_if(predators.begin(), predators.end(),
                       [](const auto& pred) { return pred->isDead() || pred->isStarving(); }),
        predators.end()
        );

    preys.erase(
        std::remove_if(preys.begin(), preys.end(),
                       [](const auto& prey) { return prey->isDead(); }),
        preys.end()
        );

    // Nettoyer nourriture consommée
    foods.erase(
        std::remove_if(foods.begin(), foods.end(),
                       [](const auto& food) { return food->consumed; }),
        foods.end()
        );

    // Update du graphique
    if (graphUpdateTimer > 0.3f) {
        float preyAvg = 0, predAvg = 0;
        if (!preys.empty()) {
            for (const auto& p : preys) preyAvg += p->fitness;
            preyAvg /= preys.size();
        }
        if (!predators.empty()) {
            for (const auto& p : predators) predAvg += p->fitness;
            predAvg /= predators.size();
        }
        graph.addData(preyAvg, predAvg);
        graphUpdateTimer = 0;
    }

    if (timer > gui.generationTime) {
        evolve();
        timer = 0;
    }
}

void Simulation::evolve() {
    ++generation;

    // Évolution proies - garder survivants
    if (!preys.empty()) {
        std::sort(preys.begin(), preys.end(),
                  [](const auto& a, const auto& b) { return a->fitness > b->fitness; });

        int survivors = std::min(8, (int)preys.size());
        std::vector<std::unique_ptr<Prey>> newGen;

        // Garder les meilleurs
        for (int i = 0; i < survivors && i < (int)preys.size(); ++i) {
            newGen.emplace_back(std::move(preys[i]));
            newGen.back()->fitness = 0;
            newGen.back()->age = 0;
        }

        // Reproduction
        for (int i = 0; i < survivors && i < (int)newGen.size(); ++i) {
            for (int j = 0; j < 2; ++j) {
                auto child = std::make_unique<Prey>(newGen[i]->pos.x + randFloat(-20, 20),
                                                    newGen[i]->pos.y + randFloat(-20, 20));
                child->brain = newGen[i]->brain->clone();
                child->brain->mutate(gui.mutationRate);
                child->generation = ++preyGeneration;
                newGen.emplace_back(std::move(child));
            }
        }
        preys = std::move(newGen);
    }

    // Réinitialiser si extinction
    if (preys.size() < 5) {
        for (int i = preys.size(); i < 15; ++i) {
            preys.push_back(std::make_unique<Prey>(randFloat(50, GUI::res_width-50), randFloat(50, GUI::res_height-50)));
        }
    }

    if (predators.size() < 2) {
        for (int i = predators.size(); i < 4; ++i) {
            predators.push_back(std::make_unique<Predator>(randFloat(50, GUI::res_width-50), randFloat(50, GUI::res_height-50)));
        }
    }
}

void Simulation::draw(sf::RenderWindow& window, const sf::Font& font) {
    // Dessiner terrain
    for (const auto& tile : terrain) {
        tile.draw(window);
    }

    // Dessiner nourriture
    for (const auto& food : foods) {
        food->draw(window);
    }
    //Dessiner la vitesse de chaque entitée
    if (gui.showAverageSpeed) {
        for (const auto& prey : preys) {
            float avgSpeed = sqrt(pow(prey->vel.x, 2) + pow(prey->vel.y, 2));
            std::stringstream ss;
            ss << avgSpeed<<std::endl;
            sf::Text speed(font);
            speed.setString(ss.str());
            speed.setCharacterSize(10);
            speed.setPosition(prey->pos);
            speed.setFillColor(sf::Color::White);
            window.draw(speed);
        }
        for (const auto& pred : predators) {
            float avgSpeed = sqrt(pow(pred->vel.x,2) + pow(pred->vel.y, 2));
            std::stringstream ss;
            ss << avgSpeed<<std::endl;
            sf::Text speed(font);
            speed.setString(ss.str());
            speed.setCharacterSize(10);
            speed.setPosition(pred->pos);
            speed.setFillColor(sf::Color::White);
            window.draw(speed);
        }
    }



    // Dessiner cercles de détection
    if (gui.showDetectionRadius) {
        for (const auto& prey : preys) {
            sf::CircleShape detectionCircle(Prey::DETECTION_RADIUS);
            detectionCircle.setPosition(prey->pos - sf::Vector2f(Prey::DETECTION_RADIUS, Prey::DETECTION_RADIUS));
            detectionCircle.setFillColor(sf::Color(0, 255, 0, 10));
            detectionCircle.setOutlineColor(sf::Color(0, 255, 0, 30));
            detectionCircle.setOutlineThickness(1);
            window.draw(detectionCircle);
        }

        for (const auto& pred : predators) {
            sf::CircleShape detectionCircle(Predator::DETECTION_RADIUS);
            detectionCircle.setPosition(pred->pos - sf::Vector2f(Predator::DETECTION_RADIUS, Predator::DETECTION_RADIUS));
            detectionCircle.setFillColor(sf::Color(255, 0, 0, 10));
            detectionCircle.setOutlineColor(sf::Color(255, 0, 0, 30));
            detectionCircle.setOutlineThickness(1);
            window.draw(detectionCircle);
        }
    }

    // Dessiner entités
    for (const auto& prey : preys)
        prey->draw(window, gui.showDirectionLines);


    for (const auto& pred : predators)
        pred->draw(window, gui.showDirectionLines);

    // Stats
    std::stringstream ss;
    ss << "Generation: " << generation << "\n"
       << "Proies: " << preys.size() << " (Gen " << preyGeneration << ")\n"
       << "Predateurs: " << predators.size() << " (Gen " << predGeneration << ")\n"
       << "Nourriture: " << foods.size() << "\n"
       << "Temps: " << std::fixed << std::setprecision(1) << timer << "s / " << (int)gui.generationTime << "s\n";

    if (!preys.empty()) {
        float avgFit = 0, avgEnergy = 0;
        for (const auto& prey : preys) {
            avgFit += prey->fitness;
            avgEnergy += prey->energy;
        }
        ss << "Proies Fitness: " << (int)(avgFit / preys.size())
           << " | E: " << (int)(avgEnergy / preys.size()) << "\n";
    }

    if (!predators.empty()) {
        float avgFit = 0;
        int totalKills = 0;
        float avgHunger = 0;
        for (const auto& pred : predators) {
            avgFit += pred->fitness;
            totalKills += pred->kills;
            avgHunger += pred->timeSinceLastMeal;
        }
        ss << "Preds Fitness: " << (int)(avgFit / predators.size()) << "\n"
           << "Captures: " << totalKills
           << " | Faim: " << std::setprecision(1) << (avgHunger / predators.size()) << "s";
    }

    sf::Text text(font);
    text.setString(ss.str());
    text.setFont(font);
    text.setCharacterSize(11);
    text.setPosition({10, 10});
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1);
    window.draw(text);

    // Dessiner graphique et GUI
    graph.draw(window, font);
    gui.draw(window, font);
}

void Simulation::handleKeyPress(sf::Keyboard::Key key) {
    gui.handleInput(key);
}
