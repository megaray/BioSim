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

inline float NeuralNetwork::sigmoidDerivative(float x) {
    // Dérivée de la sigmoid: sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
    // x est déjà passé par sigmoid, donc on calcule directement
    return x * (1.0f - x);
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
    // Sauvegarder l'input pour la backpropagation
    lastInput.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        lastInput[i] = input[i];
    }

    // Couche cachée
    lastHidden.resize(b1.size());
    for (size_t i = 0; i < lastHidden.size(); ++i) {
        float sum = b1[i];
        for (size_t j = 0; j < input.size(); ++j)
            sum += input[j] * w1[j][i];
        lastHidden[i] = NeuralNetwork::sigmoid(sum);
    }

    // Couche de sortie
    for (size_t i = 0; i < lastOutput.size(); ++i) {
        float sum = b2[i];
        for (size_t j = 0; j < lastHidden.size(); ++j)
            sum += lastHidden[j] * w2[j][i];
        lastOutput[i] = sigmoid(sum);
    }
    return lastOutput;
}

// ============================================================================
// BACKPROPAGATION - APPRENTISSAGE PAR GRADIENT DESCENT
// ============================================================================
// Cette fonction implémente l'algorithme de backpropagation pour entraîner
// le réseau neuronal. Elle calcule les gradients de l'erreur par rapport aux
// poids et biais, puis met à jour ces paramètres pour réduire l'erreur.
// ============================================================================
void NeuralNetwork::backward(const std::array<float, 2>& target, float learningRate) {
    // ========== ÉTAPE 1: CALCULER L'ERREUR DE SORTIE ==========
    // Erreur = (sortie prédite - sortie désirée)
    std::array<float, 2> outputError;
    for (size_t i = 0; i < lastOutput.size(); ++i) {
        outputError[i] = lastOutput[i] - target[i];
    }

    // ========== ÉTAPE 2: CALCULER LE GRADIENT DE SORTIE ==========
    // Gradient = erreur * dérivée de sigmoid
    std::array<float, 2> outputGradient;
    for (size_t i = 0; i < lastOutput.size(); ++i) {
        outputGradient[i] = outputError[i] * sigmoidDerivative(lastOutput[i]);
    }

    // ========== ÉTAPE 3: METTRE À JOUR LES POIDS W2 ET BIAIS B2 ==========
    // Pour chaque connexion entre la couche cachée et la sortie
    for (size_t i = 0; i < lastHidden.size(); ++i) {
        for (size_t j = 0; j < lastOutput.size(); ++j) {
            // Gradient descent: poids -= learningRate * gradient * activation
            w2[i][j] -= learningRate * outputGradient[j] * lastHidden[i];
        }
    }
    // Mettre à jour les biais de sortie
    for (size_t i = 0; i < lastOutput.size(); ++i) {
        b2[i] -= learningRate * outputGradient[i];
    }

    // ========== ÉTAPE 4: PROPAGER L'ERREUR À LA COUCHE CACHÉE ==========
    // L'erreur de la couche cachée est la somme pondérée des gradients de sortie
    std::vector<float> hiddenError(lastHidden.size(), 0.0f);
    for (size_t i = 0; i < lastHidden.size(); ++i) {
        for (size_t j = 0; j < lastOutput.size(); ++j) {
            hiddenError[i] += outputGradient[j] * w2[i][j];
        }
    }

    // ========== ÉTAPE 5: CALCULER LE GRADIENT DE LA COUCHE CACHÉE ==========
    std::vector<float> hiddenGradient(lastHidden.size());
    for (size_t i = 0; i < lastHidden.size(); ++i) {
        hiddenGradient[i] = hiddenError[i] * sigmoidDerivative(lastHidden[i]);
    }

    // ========== ÉTAPE 6: METTRE À JOUR LES POIDS W1 ET BIAIS B1 ==========
    // Pour chaque connexion entre l'input et la couche cachée
    for (size_t i = 0; i < lastInput.size(); ++i) {
        for (size_t j = 0; j < lastHidden.size(); ++j) {
            w1[i][j] -= learningRate * hiddenGradient[j] * lastInput[i];
        }
    }
    // Mettre à jour les biais de la couche cachée
    for (size_t i = 0; i < lastHidden.size(); ++i) {
        b1[i] -= learningRate * hiddenGradient[i];
    }
}



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
