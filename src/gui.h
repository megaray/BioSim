#ifndef GUI_H
#define GUI_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <deque>
#include <string>
#include <map>

class GUI
{
public:
    //========Résolution==============
    static const int res_height = 800;
    static const int res_width = 800;
    //================================
    class FitnessGraph {
    private:
        std::deque<float> preyFitness;
        std::deque<float> predatorFitness;
        static constexpr size_t MAX_POINTS = 100;

    public:
        void addData(float preyAvg, float predAvg);

        void draw(sf::RenderWindow& window, const sf::Font& font) const;
    };

    // ============ DEBUG MONITOR ============
    // Interface pour surveiller les valeurs sources de bugs
    class DebugMonitor {
    private:
        std::map<std::string, float> values;
        bool enabled;

    public:
        DebugMonitor() : enabled(false) {}

        void toggle() { enabled = !enabled; }
        bool isEnabled() const { return enabled; }

        void setValue(const std::string& name, float value) {
            values[name] = value;
        }

        void draw(sf::RenderWindow& window, const sf::Font& font) const;
    };

    // ============ GUI CONTROLS ============
    class GUIControls {
    public:
        // Flags d'affichage
        bool showDetectionRadius;
        bool showAverageSpeed;
        bool showDirectionLines;

        // Paramètres de simulation - SÉPARÉS pour éviter les conflits
        float mutationRate;
        float generationTime;
        float fastForwardRate;

        // Debug monitor
        DebugMonitor debugMonitor;

        // Frame counter pour détecter le cycling
        int frameCount;
        float lastMutationRate;
        float lastGenerationTime;
        float lastFastForwardRate;

        GUIControls()
            : showDetectionRadius(true),
              showAverageSpeed(false),
              showDirectionLines(true),
              mutationRate(0.15f),
              generationTime(30.0f),
              fastForwardRate(1.0f),
              frameCount(0),
              lastMutationRate(0.15f),
              lastGenerationTime(30.0f),
              lastFastForwardRate(1.0f) {}

        void draw(sf::RenderWindow& window, const sf::Font& font);

        void handleInput(sf::Keyboard::Key key);

        void update(); // Pour détecter les changements non autorisés
    };
};

#endif // GUI_H
