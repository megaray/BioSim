# 🐾 BioSim - Simulateur d'Écosystème avec IA

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-3.0-green.svg)](https://www.sfml-dev.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-orange.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)

**BioSim** est un simulateur d'écosystème intelligent qui met en scène des **prédateurs** et des **proies** dans un environnement dynamique. Chaque créature utilise un **réseau neuronal** pour prendre ses décisions, permettant l'émergence de comportements complexes et l'évolution de stratégies de survie.

![Demo](docs/demo.gif) <!-- Ajoutez une capture d'écran si possible -->

---

## ✨ Fonctionnalités

### 🧠 Intelligence Artificielle
- **Réseaux neuronaux** (8 inputs → 20 neurones cachés → 2 outputs)
- **Algorithme génétique** avec sélection naturelle
- **Backpropagation** pour l'apprentissage supervisé
- **Mutation** des cerveaux neuronaux à chaque génération

### 🎮 Simulation en Temps Réel
- **Physique réaliste** basée sur l'accélération et la friction
- **Gestion de l'énergie** (métabolisme, faim, vieillesse)
- **Reproduction** des meilleurs individus
- **Fast-forward** jusqu'à 100x pour accélérer l'évolution

### 📊 Monitoring et Debug
- **Graphique de fitness** en temps réel
- **Debug monitor** (F1) pour surveiller les valeurs internes
- **Affichage des vitesses** individuelles
- **Rayons de détection** visuels
- **Lignes de direction** pour comprendre les mouvements

### 🌍 Environnement (Actuellement désactivé pour debugging)
- Terrain procédural avec lacs, rivières, prairies et déserts
- Obstacles et zones à effets variables
- Spawn dynamique de nourriture

---

## 🚀 Installation

### Prérequis
- **C++17** ou supérieur
- **CMake 3.16+**
- **SFML 3.0** (téléchargé automatiquement par CMake)
- Compilateur compatible (GCC, Clang, MSVC)

### Linux/macOS
```bash
# Cloner le dépôt
git clone https://github.com/megaray/BioSim.git
cd BioSim

# Créer le dossier de build
mkdir -p build
cd build

# Configurer avec CMake
cmake ..

# Compiler
make -j$(nproc)

# Exécuter
./BioSim
```

### Windows (avec Visual Studio)
```bash
# Cloner le dépôt
git clone https://github.com/megaray/BioSim.git
cd BioSim

# Générer le projet Visual Studio
cmake -B build -G "Visual Studio 17 2022"

# Ouvrir le fichier .sln dans Visual Studio
# Ou compiler en ligne de commande :
cmake --build build --config Release
```

### Qt Creator
1. Ouvrir `CMakeLists.txt` dans Qt Creator
2. Configurer le kit de compilation
3. Compiler et exécuter (Ctrl+R)

---

## 🎮 Contrôles

| Touche | Action |
|--------|--------|
| **F1** | Activer/Désactiver le Debug Monitor |
| **D** | Afficher/Masquer les rayons de détection |
| **L** | Afficher/Masquer les lignes de direction |
| **V** | Afficher/Masquer les vitesses |
| **←/→** | Diminuer/Augmenter le taux de mutation |
| **↑/↓** | Augmenter/Diminuer le temps de génération |
| **Q/W** | Diminuer/Augmenter le fast-forward |

Pour plus de détails, voir [CONTROLS.md](docs/CONTROLS.md)

---

## 📐 Architecture

Le projet est organisé en modules clairs :

```
BioSim/
├── src/
│   ├── main.cpp              # Point d'entrée
│   ├── simulation.cpp/h      # Gestion de l'écosystème
│   ├── entity.cpp/h          # Entités (proies, prédateurs)
│   ├── neuralnetwork.cpp/h   # Réseau neuronal + backpropagation
│   ├── gui.cpp/h             # Interface utilisateur + debug monitor
│   ├── terraintype.cpp/h     # Gestion du terrain
│   └── survivallogic.cpp/h   # Logique de survie (nourriture, etc.)
├── CMakeLists.txt            # Configuration CMake
├── README.md                 # Ce fichier
├── docs/
│   ├── ARCHITECTURE.md       # Documentation technique
│   └── CONTROLS.md           # Guide des contrôles
└── .gitignore
```

Pour une explication détaillée de l'architecture, voir [ARCHITECTURE.md](docs/ARCHITECTURE.md)

---

## 🧬 Comment ça marche ?

### 1. Les Créatures
- **Proies** (vertes) : Cherchent la nourriture et fuient les prédateurs
- **Prédateurs** (rouges) : Chassent les proies pour survivre

### 2. Le Cerveau Neuronal
Chaque créature possède un réseau neuronal qui reçoit :
- Distance au prédateur/proie le plus proche (x, y)
- Distance à la nourriture la plus proche (x, y)
- Niveau d'énergie, âge, vitesse actuelle

Et produit :
- Direction de mouvement (angle)
- Intensité de l'accélération

### 3. L'Évolution
Toutes les 30 secondes (modifiable) :
1. Les créatures sont classées par fitness (survie, reproduction, captures)
2. Les **8 meilleures** survivent
3. Chacune se reproduit 2 fois avec **mutation** de son cerveau
4. Les cerveaux mutés explorent de nouvelles stratégies

### 4. La Backpropagation (Nouvelle fonctionnalité !)
En complément de l'évolution génétique, les cerveaux peuvent désormais **apprendre** :
- Calcul des gradients par rétropropagation
- Ajustement des poids pour réduire l'erreur
- Apprentissage supervisé ou par renforcement

---

## 📊 Statistiques et Monitoring

### Affichage Principal
- **Génération actuelle** de l'écosystème
- **Population** de proies et prédateurs
- **Fitness moyenne** de chaque espèce
- **Graphique d'évolution** de la fitness

### Debug Monitor (F1)
- Valeurs des paramètres en temps réel
- Détection de changements anormaux (cycling)
- Compteur de frames
- Taux de mutation, temps de génération, fast-forward

---

## 🔧 Configuration Avancée

### Modifier les Paramètres de Simulation

Dans `src/simulation.cpp`, vous pouvez modifier :
```cpp
// Nombre initial de créatures
preys.reserve(25);      // Nombre de proies
predators.reserve(6);   // Nombre de prédateurs

// Temps de génération
gui.generationTime = 30.0f;  // en secondes

// Taux de mutation
gui.mutationRate = 0.15f;    // entre 0.0 et 1.0
```

### Modifier le Réseau Neuronal

Dans `src/entity.cpp` :
```cpp
// Proie : 8 inputs → 20 hidden → 2 outputs
Prey::Prey(float x, float y) : Entity(x, y, 5, sf::Color::Green, 8, 20, 2) {}

// Prédateur : 8 inputs → 20 hidden → 2 outputs
Predator::Predator(float x, float y) : Entity(x, y, 8, sf::Color::Red, 8, 20, 2) {}
```

---

## 🐛 Problèmes Connus et Développement

### ✅ Récemment Corrigé
- [x] Bug de cycling des valeurs UI
- [x] Problème de vitesse et mémoire
- [x] Confusion entre mutation rate et generation time
- [x] Ajout du debug monitor
- [x] Implémentation de la backpropagation

### 🚧 En Cours
- [ ] Ré-activation du terrain avec gestion optimisée
- [ ] Amélioration de l'algorithme génétique
- [ ] Ajout de statistiques d'évolution avancées
- [ ] Mode "replay" pour revoir les générations passées

### 💡 Améliorations Futures
- [ ] Sauvegarde/Chargement des cerveaux entraînés
- [ ] Export des données en CSV pour analyse
- [ ] Interface graphique pour modifier les paramètres en live
- [ ] Multi-threading pour des simulations plus grandes
- [ ] Nouveaux types de créatures (omnivores, plantes)

---

## 🤝 Contribution

Les contributions sont les bienvenues ! Voici comment contribuer :

1. **Fork** le projet
2. Créez une **branche** pour votre fonctionnalité (`git checkout -b feature/AmazingFeature`)
3. **Committez** vos changements (`git commit -m 'Add some AmazingFeature'`)
4. **Push** vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une **Pull Request**

---

## 📝 Licence

Ce projet est sous licence MIT. Voir [LICENSE.md](LICENSE.md) pour plus de détails.

---

## 🙏 Remerciements

- **SFML** pour la bibliothèque graphique
- La communauté C++ pour les ressources et l'inspiration
- Tous les contributeurs du projet

---

## 📧 Contact

Pour toute question ou suggestion :
- **GitHub Issues** : [https://github.com/megaray/BioSim/issues](https://github.com/megaray/BioSim/issues)
- **Auteur** : megaray

---

## 🎓 Ressources pour Apprendre

### Réseaux Neuronaux
- [Neural Networks and Deep Learning](http://neuralnetworksanddeeplearning.com/) par Michael Nielsen
- [3Blue1Brown - Neural Networks](https://www.youtube.com/playlist?list=PLZHQObOWTQDNU6R1_67000Dx_ZCJB-3pi)

### Algorithmes Génétiques
- [Introduction to Genetic Algorithms](https://www.youtube.com/watch?v=9zfeTw-uFCw)
- [The Nature of Code](https://natureofcode.com/) par Daniel Shiffman

### SFML
- [Documentation officielle SFML](https://www.sfml-dev.org/learn.php)

---

**Bon codage et bonne évolution ! 🧬🚀**
