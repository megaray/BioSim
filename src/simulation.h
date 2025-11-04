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

// ============================================================================
// SIMULATION - Gère l'écosystème complet (proies, prédateurs, nourriture)
// ============================================================================
// ARCHITECTURE CORRIGÉE: La classe Simulation ne possède PLUS sa propre
// instance de GUI. Elle reçoit une RÉFÉRENCE à l'instance unique créée
// dans main.cpp. Cela évite les problèmes de synchronisation entre
// plusieurs instances de GUI qui causaient le bug de cycling.
// ============================================================================

class Simulation {
private:
    // ========== ENTITÉS ET ENVIRONNEMENT ==========
    std::vector<std::unique_ptr<Prey>> preys;
    std::vector<std::unique_ptr<Predator>> predators;
    std::vector<std::unique_ptr<Food>> foods;
    std::vector<TerrainTile> terrain;

    // ========== GESTION DU TEMPS ET DES GÉNÉRATIONS ==========
    int generation;
    float timer;
    int preyGeneration, predGeneration;

    // ========== GRAPHIQUES ET INTERFACE ==========
    GUI::FitnessGraph graph;

    // IMPORTANT: RÉFÉRENCE vers l'instance unique de GUI créée dans main.cpp
    // Cela garantit qu'il n'y a qu'UNE SEULE source de vérité pour les paramètres
    GUI::GUIControls& gui;

    // ========== TIMERS ==========
    float graphUpdateTimer;
    float foodSpawnTimer;

    // ========== FONCTIONS PRIVÉES ==========
    static std::mt19937& getRNG();
    static float randFloat(float min, float max);
    static int randInt(int min, int max);
    void generateTerrain();
    void spawnFood();

public:
    // CONSTRUCTEUR: Prend une RÉFÉRENCE à l'instance GUI unique
    // Cette référence est stockée et utilisée tout au long de la simulation
    Simulation(GUI::GUIControls& guiControls);

    // Met à jour la simulation à chaque frame
    void update(float dt);

    // Fait évoluer les populations (sélection naturelle)
    void evolve();

    // Dessine tout sur l'écran
    void draw(sf::RenderWindow& window, const sf::Font& font);

    // Gère les touches du clavier
    void handleKeyPress(sf::Keyboard::Key key);
};
#endif // SIMULATION_H
