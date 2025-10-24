#ifndef GUI_H
#define GUI_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <deque>

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

    // ============ GUI CONTROLS ============
    class GUIControls {
    public:
        bool showDetectionRadius;
        bool showAverageSpeed;
        bool showDirectionLines;
        float mutationRate;
        float generationTime;
        float fastForwardRate;

        GUIControls()
            : showDetectionRadius(true),
              showAverageSpeed(true),
              showDirectionLines(true),
              mutationRate(0.15f),
              generationTime(30.0f),
              fastForwardRate(1.0f) {}

        void draw(sf::RenderWindow& window, const sf::Font& font);

        void handleInput(sf::Keyboard::Key key);
    };
};

#endif // GUI_H
