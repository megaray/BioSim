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

    // Cache pour la backpropagation
    std::vector<float> lastInput;
    std::vector<float> lastHidden;
    std::array<float, 2> lastOutput;

    static std::mt19937& getRNG();
    static inline float sigmoid(float x);
    static inline float sigmoidDerivative(float x);
    static float randomWeight();
public:
    NeuralNetwork(int input, int hidden, int output);
    std::array<float, 2> forward(const std::array<float, 8>& input);

    // Backpropagation: calcule les gradients et met à jour les poids
    // target: la sortie désirée
    // learningRate: taux d'apprentissage (ex: 0.01)
    void backward(const std::array<float, 2>& target, float learningRate);

    void mutate(float rate);
    std::unique_ptr<NeuralNetwork> clone() const;
};

#endif // NEURALNETWORK_H
