# 🏗️ Architecture de BioSim

Ce document décrit l'architecture technique du projet BioSim, son organisation et les décisions de conception.

---

## 📋 Vue d'Ensemble

BioSim suit une architecture **modulaire** avec séparation claire des responsabilités :

```
┌─────────────────────────────────────────────┐
│              main.cpp                       │
│  (Boucle principale, gestion événements)    │
└────────────┬────────────────────────────────┘
             │
    ┌────────┴─────────┐
    │   Simulation     │──────┐
    │  (Écosystème)    │      │
    └──────┬───────────┘      │
           │                  │
    ┌──────┴────────┐    ┌────┴─────┐
    │    Entity     │    │   GUI    │
    │  (Créatures)  │    │ (UI/HUD) │
    └──────┬────────┘    └──────────┘
           │
    ┌──────┴──────────┐
    │ NeuralNetwork   │
    │ (Cerveaux IA)   │
    └─────────────────┘
```

---

## 🗂️ Modules Principaux

### 1. **main.cpp** - Point d'Entrée
**Responsabilités** :
- Initialiser SFML (fenêtre, police, horloge)
- Créer l'instance unique de `GUI::GUIControls`
- Créer la simulation avec référence au GUI
- Boucle de jeu principale :
  - Gestion des événements (clavier, fermeture)
  - Calcul du delta time
  - Mise à jour du GUI
  - Application du fast-forward
  - Mise à jour de la simulation
  - Rendu

**Décision de conception** :
- Une **seule instance** de `GUI::GUIControls` créée dans `main.cpp`
- Passée par **référence** à `Simulation`
- Évite les problèmes de synchronisation et de cycling

---

### 2. **simulation.cpp/h** - Gestion de l'Écosystème

**Classe** : `Simulation`

**Attributs** :
```cpp
// Entités
std::vector<std::unique_ptr<Prey>> preys;
std::vector<std::unique_ptr<Predator>> predators;
std::vector<std::unique_ptr<Food>> foods;
std::vector<TerrainTile> terrain;  // Actuellement désactivé

// Gestion du temps
int generation;
float timer;
float graphUpdateTimer;
float foodSpawnTimer;

// Référence à l'UI (UNIQUE SOURCE DE VÉRITÉ)
GUI::GUIControls& gui;

// Graphique de fitness
GUI::FitnessGraph graph;
```

**Méthodes principales** :
- `update(float dt)` : Met à jour toutes les entités, gère les collisions, la nourriture
- `evolve()` : Algorithme génétique (sélection, reproduction, mutation)
- `draw(...)` : Affiche tout sur l'écran
- `handleKeyPress(...)` : Délègue au GUI

**Flux de l'update** :
```
1. Incrémenter les timers
2. Spawn périodique de nourriture (tous les 5s)
3. Update des proies (think + update + manger)
4. Update des prédateurs (think + update)
5. Détection des captures (prédateur mange proie)
6. Suppression des morts (faim, vieillesse)
7. Suppression nourriture consommée
8. Update graphique (tous les 0.3s)
9. Évolution si timer > generationTime
```

---

### 3. **entity.cpp/h** - Entités Vivantes

**Hiérarchie** :
```
Entity (classe de base)
├── Prey (proie)
└── Predator (prédateur)
```

**Classe `Entity`** :

**Attributs physiques** :
```cpp
sf::Vector2f pos;      // Position
sf::Vector2f vel;      // Vélocité
sf::Vector2f acc;      // Accélération
float radius;          // Taille
float maxSpeed;        // Vitesse maximale
```

**Attributs biologiques** :
```cpp
float energy;              // Énergie vitale
float fitness;             // Score évolutif
int age;                   // Âge (en frames)
float timeSinceLastMeal;   // Temps depuis dernier repas
int generation;            // Numéro de génération
```

**Système physique** :
```cpp
void update(float dt, ...) {
    // 1. Appliquer accélération → vitesse
    vel += acc * dt;

    // 2. Friction (résistance)
    vel *= 0.98f;

    // 3. Limiter la vitesse max
    if (speed > maxSpeed) vel *= (maxSpeed / speed);

    // 4. Seuil minimum (éviter micro-mouvements)
    if (speed < 1.0f) vel = {0, 0};

    // 5. Réinitialiser accélération
    acc = {0, 0};

    // 6. Appliquer vitesse → position
    pos += vel * dt;

    // 7. Wrap-around (bords de carte)
    // 8. Terrain (actuellement désactivé)
    // 9. Métabolisme
}
```

**Classe `Prey`** :

**Inputs du cerveau** (8) :
```cpp
1. closestPredDist (normalisé)
2. (closestPred.x - pos.x) / DETECTION_RADIUS
3. (closestPred.y - pos.y) / DETECTION_RADIUS
4. closestFoodDist (normalisé)
5. (closestFood.x - pos.x) / DETECTION_RADIUS
6. (closestFood.y - pos.y) / DETECTION_RADIUS
7. energy / 100.0f
8. age / 5400.0f
```

**Outputs** (2) :
```cpp
1. Angle de direction (0-1 → 0-2π)
2. Force d'accélération (0-1 → 0-maxAccel)
```

**Classe `Predator`** :
- Même structure, mais traque les proies au lieu de la nourriture
- Plus grand rayon de détection (100 vs 80)
- Système de "faim" (meurt après 20s sans manger)

---

### 4. **neuralnetwork.cpp/h** - Cerveau IA

**Architecture** :
```
Input Layer (8)
     ↓
Hidden Layer (20)  [activation: sigmoid]
     ↓
Output Layer (2)   [activation: sigmoid]
```

**Classe `NeuralNetwork`** :

**Poids et biais** :
```cpp
std::vector<std::vector<float>> w1;  // [input][hidden]
std::vector<std::vector<float>> w2;  // [hidden][output]
std::vector<float> b1;               // hidden biases
std::vector<float> b2;               // output biases
```

**Forward Pass** :
```cpp
hidden[i] = sigmoid(Σ(input[j] × w1[j][i]) + b1[i])
output[i] = sigmoid(Σ(hidden[j] × w2[j][i]) + b2[i])
```

**Backpropagation** (nouvelle fonctionnalité) :
```cpp
void backward(target, learningRate) {
    // 1. Calculer erreur de sortie
    outputError = output - target

    // 2. Gradient de sortie
    outputGradient = outputError × sigmoid'(output)

    // 3. Mettre à jour W2 et b2
    W2 -= learningRate × outputGradient × hidden
    b2 -= learningRate × outputGradient

    // 4. Propager erreur vers couche cachée
    hiddenError = Σ(outputGradient × W2)

    // 5. Gradient caché
    hiddenGradient = hiddenError × sigmoid'(hidden)

    // 6. Mettre à jour W1 et b1
    W1 -= learningRate × hiddenGradient × input
    b1 -= learningRate × hiddenGradient
}
```

**Mutation** :
```cpp
void mutate(rate) {
    for (poids in W1, W2) {
        if (random() < rate) {
            poids += randomWeight() × 0.5
        }
    }
}
```

**Clone** :
- Copie profonde de tous les poids
- Utilisé pour la reproduction

---

### 5. **gui.cpp/h** - Interface Utilisateur

**Classes** :

#### `GUI::FitnessGraph`
Affiche l'évolution de la fitness sur 100 points.

**Attributs** :
```cpp
std::deque<float> preyFitness;
std::deque<float> predatorFitness;
```

**Rendu** : Courbes vertes (proies) et rouges (prédateurs)

#### `GUI::DebugMonitor` (nouveau)
Surveille les valeurs sources de bugs.

**Attributs** :
```cpp
std::map<std::string, float> values;
bool enabled;
```

**Utilisation** :
```cpp
debugMonitor.setValue("mutationRate", 0.15f);
debugMonitor.setValue("frameCount", 1234);
```

**Affichage** : Panneau cyan en bas à gauche avec toutes les valeurs

#### `GUI::GUIControls`
**SOURCE DE VÉRITÉ UNIQUE** pour tous les paramètres de simulation.

**Attributs** :
```cpp
// Paramètres modifiables
float mutationRate;       // Taux de mutation
float generationTime;     // Durée d'une génération
float fastForwardRate;    // Multiplicateur de vitesse

// Flags d'affichage
bool showDetectionRadius;
bool showAverageSpeed;
bool showDirectionLines;

// Détection de cycling
float lastMutationRate;
float lastGenerationTime;
float lastFastForwardRate;
int frameCount;

// Debug monitor
DebugMonitor debugMonitor;
```

**Méthodes** :
- `handleInput(key)` : **SEUL ENDROIT** où les valeurs sont modifiées
- `update()` : Détecte les changements non autorisés (cycling)
- `draw()` : Affiche l'UI sans JAMAIS modifier les valeurs

**Anti-pattern évité** :
```cpp
// ❌ NE JAMAIS FAIRE ÇA :
void draw() {
    mutationRate = 0.15f;  // INTERDIT : réinitialise à chaque frame !
}

// ✅ CORRECT :
void draw() {
    // Juste afficher, ne pas modifier
    text.setString("Mutation: " + mutationRate);
}
```

---

### 6. **terraintype.cpp/h** - Gestion du Terrain

**Actuellement désactivé** pour faciliter le debugging.

**Types de terrain** :
```cpp
enum class TerrainType {
    GRASS,   // Prairie (+ nourriture)
    WATER,   // Eau (- vitesse)
    DESERT   // Désert (- énergie)
};
```

**Classe `TerrainTile`** :
```cpp
TerrainType type;
sf::ConvexShape shape;              // Polygone
std::vector<sf::ConvexShape> obstacles;  // Rochers, arbres
```

---

### 7. **survivallogic.cpp/h** - Logique de Survie

**Classe `Food`** :
```cpp
sf::Vector2f pos;
float energy = 50.0f;
bool consumed = false;
```

Simple représentation de nourriture spawnable.

---

## 🔄 Flux de Données

### Cycle de Vie d'une Frame

```
1. main.cpp : pollEvent()
   ↓
2. main.cpp : sim.handleKeyPress()
   ↓
3. gui.handleInput() → Modifie mutationRate/etc
   ↓
4. main.cpp : gui.update() → Détecte cycling
   ↓
5. main.cpp : dt *= gui.fastForwardRate
   ↓
6. sim.update(dt)
   ├─→ prey.think() → brain.forward()
   ├─→ prey.update() → Physique
   ├─→ predator.think() → brain.forward()
   ├─→ predator.update() → Physique
   └─→ Collisions, morts, évolution
   ↓
7. sim.draw()
   ├─→ entity.draw()
   ├─→ graph.draw()
   └─→ gui.draw() → debugMonitor.draw()
   ↓
8. window.display()
```

### Cycle d'Évolution

```
Timer atteint generationTime
   ↓
sim.evolve()
   ↓
1. Trier par fitness
   ↓
2. Garder les 8 meilleurs
   ↓
3. Pour chaque survivant :
   ├─→ Cloner le cerveau
   ├─→ Muter avec gui.mutationRate
   ├─→ Créer 2 enfants
   └─→ Incrémenter generation
   ↓
4. Réinitialiser si extinction
```

---

## 🛡️ Décisions de Conception Importantes

### 1. **Référence GUI Unique**
**Problème** : Plusieurs instances de GUI causaient du cycling.

**Solution** : Une seule instance dans `main.cpp`, passée par référence.

**Avantages** :
- Une seule source de vérité
- Pas de désynchronisation
- Facile à débugger

### 2. **Physique Basée sur l'Accélération**
**Pourquoi pas directement la vitesse ?**
- Plus réaliste (inertie)
- Mouvements plus naturels
- Meilleur pour l'apprentissage IA

### 3. **Smart Pointers**
```cpp
std::vector<std::unique_ptr<Prey>> preys;
```
- Gestion automatique de la mémoire
- Pas de fuites
- Ownership clair

### 4. **Désactivation du Terrain**
- Simplifie le debugging
- Réduit la complexité
- Sera réactivé après optimisation

---

## 🔧 Points d'Extension

### Ajouter un Nouveau Type de Créature

1. Créer une classe dérivée de `Entity`
2. Implémenter `think()` avec les inputs appropriés
3. Ajouter dans `Simulation::update()` et `evolve()`

### Modifier l'Architecture du Réseau

Dans `entity.cpp` :
```cpp
// Changer (input, hidden, output)
Prey::Prey(float x, float y)
    : Entity(x, y, 5, sf::Color::Green, 8, 20, 2) {}
    //                                   ^   ^   ^
    //                                   |   |   └─ outputs
    //                                   |   └───── hidden
    //                                   └───────── inputs
```

### Ajouter un Paramètre GUI

1. Ajouter dans `GUI::GUIControls`
2. Initialiser dans le constructeur
3. Ajouter dans `handleInput()` avec une touche dédiée
4. Ajouter dans `update()` pour tracking
5. Afficher dans `draw()`

---

## 📊 Complexité et Performance

### Complexité Temporelle

**Par frame** :
- Update des entités : O(N) où N = nombre total de créatures
- Détection des captures : O(P × D) où P = prédateurs, D = proies
- Graphique : O(1) (déjà limité à 100 points)

**Évolution** :
- Tri : O(N log N)
- Clone/Mutation : O(N × W) où W = nombre de poids

### Optimisations Possibles

- [ ] Quadtree pour la détection spatiale
- [ ] Multi-threading (update des entités en parallèle)
- [ ] Pool d'objets pour éviter allocations
- [ ] Batch rendering pour SFML

---

## 🧪 Tests et Validation

### Valider que le Cycling est Résolu

Activer le Debug Monitor (F1) et vérifier que :
- `mutation_change`, `genTime_change`, `ff_change` restent à 0
- Les valeurs ne changent QUE lors d'un appui de touche

### Valider la Backpropagation

```cpp
// Test simple
NeuralNetwork brain(8, 20, 2);
std::array<float, 8> input = {1, 0, 0, 0, 0, 0, 0, 0};
std::array<float, 2> target = {1, 0};

auto output1 = brain.forward(input);
brain.backward(target, 0.01f);
auto output2 = brain.forward(input);

// output2 devrait être plus proche de target que output1
```

---

**Architecture maintenue par** : megaray
**Dernière mise à jour** : 04/11/2025
