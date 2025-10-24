#ifndef SIMULATION_H
#define SIMULATION_H
#include "entity.h"
#include "gui.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <memory>


class Simulation {
private:
    std::vector<std::unique_ptr<Prey>> preys;
    std::vector<std::unique_ptr<Predator>> predators;
    std::vector<std::unique_ptr<Food>> foods;
    std::vector<TerrainTile> terrain;
    int generation;
    float timer;
    int preyGeneration, predGeneration;
    GUI::FitnessGraph graph;
    float graphUpdateTimer;
    float foodSpawnTimer;
    GUI::GUIControls gui;

    static std::mt19937& getRNG();

    static float randFloat(float min, float max);

    static int randInt(int min, int max);

    void generateTerrain();

    void spawnFood();

public:
    Simulation();

    void update(float dt);

    void evolve();

    void draw(sf::RenderWindow& window, const sf::Font& font);

    void handleKeyPress(sf::Keyboard::Key key);
};
#endif // SIMULATION_H
