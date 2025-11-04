#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include "gui.h"
#include "simulation.h"

// ============================================================================
// MAIN - POINT D'ENTRÉE DE L'APPLICATION
// ============================================================================
// ARCHITECTURE CORRIGÉE:
// - Une SEULE instance de GUI::GUIControls est créée ici
// - Cette instance unique est passée à Simulation par RÉFÉRENCE
// - Les modifications utilisateur sont appliquées directement à cette instance
// - Simulation utilise cette même instance pour lire les paramètres
// - RÉSULTAT: Pas de désynchronisation, pas de cycling !
// ============================================================================

int main() {
    // ========== CRÉER L'INSTANCE UNIQUE DE GUI ==========
    // IMPORTANT: C'est la SEULE instance de GUI dans toute l'application
    // Toutes les autres classes l'utilisent par référence
    GUI::GUIControls gui;

    // ========== CRÉER LA FENÊTRE SFML ==========
    sf::RenderWindow window(
        sf::VideoMode({GUI::res_width, GUI::res_height}),
        "Simulation IA Ecosystem - Proies vs Predateurs"
    );
    window.setFramerateLimit(60);

    // ========== CHARGER LA POLICE ==========
    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Font not found. Using default rendering.\n";
    }

    // ========== CRÉER LA SIMULATION ==========
    // IMPORTANT: On passe l'instance unique de GUI par référence
    // Simulation stocke cette référence et l'utilise pour tous les paramètres
    Simulation sim(gui);

    // ========== HORLOGE POUR LE DELTA TIME ==========
    sf::Clock clock;

    // ========== BOUCLE PRINCIPALE ==========
    while (window.isOpen()) {
        // ========== GESTION DES ÉVÉNEMENTS ==========
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // Transmettre les touches à Simulation qui les passe au GUI
                sim.handleKeyPress(keyPressed->code);
            }
        }

        // ========== CALCUL DU DELTA TIME ==========
        // Temps écoulé depuis la dernière frame
        float dt = clock.restart().asSeconds();

        // ========== APPLICATION DU FAST-FORWARD ==========
        // IMPORTANT: On utilise l'instance UNIQUE de GUI
        // Le multiplicateur fastForwardRate est modifié par l'utilisateur
        // via les touches Q/W et appliqué ici au delta time
        // Cela accélère toute la simulation uniformément
        dt *= gui.fastForwardRate;

        // ========== MISE À JOUR DE LA SIMULATION ==========
        // La simulation utilise la MÊME instance GUI (par référence)
        // pour lire generationTime, showAverageSpeed, etc.
        sim.update(dt);

        // ========== RENDU ==========
        // Effacer l'écran avec une couleur de fond sombre
        window.clear(sf::Color(20, 20, 30));

        // Dessiner toute la simulation (terrain, entités, UI)
        sim.draw(window, font);

        // Afficher à l'écran
        window.display();
    }

    return 0;
}
