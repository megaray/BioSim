#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <memory>
#include <array>

class NeuralNetwork
{
private:
    std::vector<std::vector<float>> w1, w2;
    std::vector<float> b1, b2;
    static std::mt19937& getRNG();
    static inline float sigmoid(float x);
    static float randomWeight();
public:
    NeuralNetwork(int input, int hidden, int output);
    std::array<float, 2> forward(const std::array<float, 8>& input);
    void mutate(float rate);
    std::unique_ptr<NeuralNetwork> clone() const;
};

#endif // NEURALNETWORK_H
