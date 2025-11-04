#include "gui.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <deque>




// ============ GRAPHIQUE DE FITNESS ============
void GUI::FitnessGraph::addData(float preyAvg, float predAvg) {
    preyFitness.push_back(preyAvg);
    predatorFitness.push_back(predAvg);

    if (preyFitness.size() > MAX_POINTS)
        preyFitness.pop_front();
    if (predatorFitness.size() > MAX_POINTS)
        predatorFitness.pop_front();
}

void GUI::FitnessGraph::draw(sf::RenderWindow& window, const sf::Font& font) const {
    const float graphX = 10.0f;
    const float graphY = 420.0f;
    const float graphWidth = 250.0f;
    const float graphHeight = 120.0f;

    sf::RectangleShape background({graphWidth, graphHeight});
    background.setPosition({graphX, graphY});
    background.setFillColor(sf::Color(40, 40, 50, 200));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(1);
    window.draw(background);

    sf::Text title(font);
    title.setString("Efficacite");
    title.setCharacterSize(10);
    title.setPosition({graphX + 5, graphY + 5});
    title.setFillColor(sf::Color::White);
    window.draw(title);

    if (preyFitness.empty()) return;

    float maxFitness = 1.0f;
    for (float f : preyFitness)
        maxFitness = std::max(maxFitness, f);
    for (float f : predatorFitness)
        maxFitness = std::max(maxFitness, f);

    const float plotY = graphY + 25.0f;
    const float plotHeight = graphHeight - 30.0f;

    auto drawCurve = [&](const std::deque<float>& data, sf::Color color) {
        if (data.size() < 2) return;

        std::vector<sf::Vertex> vertices;
        for (size_t i = 0; i < data.size(); ++i) {
            float x = graphX + (i * graphWidth / MAX_POINTS);
            float normalizedFitness = data[i] / maxFitness;
            float y = plotY + plotHeight - (normalizedFitness * plotHeight);
            vertices.push_back(sf::Vertex{{x, y}, color});
        }
        window.draw(vertices.data(),vertices.size(), sf::PrimitiveType::LineStrip);
    };

    drawCurve(preyFitness, sf::Color::Green);
    drawCurve(predatorFitness, sf::Color::Red);
}

// ============ GUI CONTROLS ============
void GUI::GUIControls::draw(sf::RenderWindow& window, const sf::Font& font) {
    // Les valeurs sont initialisées dans le constructeur et modifiées par handleInput()
    // Ne PAS réinitialiser ici pour permettre la persistance des changements utilisateur
    const float guiX = res_width - 220;
    const float guiY = 10;

    sf::RectangleShape bg({210, 200});
    bg.setPosition({guiX, guiY});
    bg.setFillColor(sf::Color(30, 30, 40, 200));
    bg.setOutlineColor(sf::Color::White);
    bg.setOutlineThickness(1);
    window.draw(bg);

    sf::Text title(font);
    title.setString("=== CONTROLES ===");
    title.setCharacterSize(12);
    title.setPosition({guiX + 10, guiY + 5});
    title.setFillColor(sf::Color::Yellow);
    window.draw(title);

    std::stringstream ss;
    ss << "\n[D] Detection: " << (showDetectionRadius ? "ON" : "OFF")
       << "\n[L] Lignes: " << (showDirectionLines ? "ON" : "OFF")
       << "\n\n[<-/->] Mutation: " << std::fixed << std::setprecision(2) << mutationRate
       << "\n[up/down] Gen Time: " << (int)generationTime << "s"
       << "\n[Q/W] fast forward" << std::fixed << std::setprecision(2) << fastForwardRate
       << "\n[V] Afficher vitesse entitées"
       << "\n\nLegende Terrain:"
       << "\nBleu = Eau (-vitesse)"
       << "\nVert = Prairie (+food)"
       << "\nJaune = Desert (-energie)";

    sf::Text controls(font);
    controls.setString(ss.str());
    controls.setCharacterSize(10);
    controls.setPosition({guiX + 10, guiY + 25});
    controls.setFillColor(sf::Color::White);
    window.draw(controls);
}

void GUI::GUIControls::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::D) {
        showDetectionRadius = !showDetectionRadius;
    }
    else if (key == sf::Keyboard::Key::L) {
        showDirectionLines = !showDirectionLines;
    }
    else if (key == sf::Keyboard::Key::Right) { // +
        mutationRate = std::min(0.5f, mutationRate + 0.05f);
    }
    else if (key == sf::Keyboard::Key::Left) { // -
        mutationRate = std::max(0.05f, mutationRate - 0.05f);
    }
    else if (key == sf::Keyboard::Key::W) { // +
        fastForwardRate = std::min(100.0f, fastForwardRate + 1.0f);
    }
    else if (key == sf::Keyboard::Key::Q) { // -
        fastForwardRate = std::max(1.0f, fastForwardRate - 1.0f);
    }
    else if (key == sf::Keyboard::Key::Down) {
        generationTime = std::max(10.0f, generationTime - 5.0f);
    }
    else if (key == sf::Keyboard::Key::Up) {
        generationTime = std::min(60.0f, generationTime + 5.0f);
    }
    else if (key == sf::Keyboard::Key::V) {
        showAverageSpeed = !showAverageSpeed;
    }

}
