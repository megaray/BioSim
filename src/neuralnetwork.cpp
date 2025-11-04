#include "neuralnetwork.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <memory>
#include <array>

std::mt19937& NeuralNetwork::getRNG() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

inline float NeuralNetwork::sigmoid(float x) {
    //redresseur (check les valeur pour voir si ca change quelquechose)
    x = std::max(-10.0f, std::min(10.0f, x));
    return 1.0f / (1.0f + std::exp(-x));
}

float NeuralNetwork::randomWeight() {
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(getRNG());
}

NeuralNetwork::NeuralNetwork(int input, int hidden, int output) {
    w1.resize(input, std::vector<float>(hidden));
    w2.resize(hidden, std::vector<float>(output));
    b1.resize(hidden);
    b2.resize(output);

    for (auto& row : w1)
        for (auto& w : row)
            w = randomWeight();

    for (auto& row : w2)
        for (auto& w : row)
            w = randomWeight();

    for (auto& b : b1)
        b = randomWeight();

    for (auto& b : b2)
        b = randomWeight();
}
std::array<float, 2> NeuralNetwork::forward(const std::array<float, 8>& input) {
    std::vector<float> hidden(b1.size());

    for (size_t i = 0; i < hidden.size(); ++i) {
        float sum = b1[i];
        for (size_t j = 0; j < input.size(); ++j)
            sum += input[j] * w1[j][i];
        hidden[i] = NeuralNetwork::sigmoid(sum);
    }

    std::array<float, 2> output;
    for (size_t i = 0; i < output.size(); ++i) {
        float sum = b2[i];
        for (size_t j = 0; j < hidden.size(); ++j)
            sum += hidden[j] * w2[j][i];
        output[i] = sigmoid(sum);
    }
    return output;
}

// Backward propagation


void NeuralNetwork::mutate(float rate) {
    static std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    auto& rng = getRNG();

    for (auto& row : w1)
        for (auto& w : row)
            if (prob(rng) < rate)
                w += randomWeight() * 0.5f;

    for (auto& row : w2)
        for (auto& w : row)
            if (prob(rng) < rate)
                w += randomWeight() * 0.5f;
}

std::unique_ptr<NeuralNetwork> NeuralNetwork::clone() const {
    auto copy = std::make_unique<NeuralNetwork>((int)w1.size(), (int)w1[0].size(), (int)w2[0].size());
    copy->w1 = w1;
    copy->w2 = w2;
    copy->b1 = b1;
    copy->b2 = b2;
    return copy;
}
