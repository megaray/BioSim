# 🎮 Guide des Contrôles - BioSim

Ce document détaille tous les contrôles disponibles dans BioSim et leur fonctionnement.

---

## ⌨️ Contrôles Principaux

### 🔍 Affichage et Visualisation

| Touche | Fonction | Description |
|--------|----------|-------------|
| **F1** | Toggle Debug Monitor | Active/désactive le panneau de debugging (cyan) en bas à gauche |
| **D** | Toggle Detection Radius | Affiche/masque les cercles de détection autour des créatures |
| **L** | Toggle Direction Lines | Affiche/masque les lignes indiquant la direction de mouvement |
| **V** | Toggle Velocity Display | Affiche/masque la vitesse numérique au-dessus de chaque créature |

### 🧬 Paramètres d'Évolution

| Touche | Fonction | Valeur | Description |
|--------|----------|--------|-------------|
| **←** (Left) | Diminuer Mutation | -0.05 | Réduit le taux de mutation (min: 0.05) |
| **→** (Right) | Augmenter Mutation | +0.05 | Augmente le taux de mutation (max: 0.50) |
| **↓** (Down) | Diminuer Gen Time | -5s | Réduit le temps d'une génération (min: 10s) |
| **↑** (Up) | Augmenter Gen Time | +5s | Augmente le temps d'une génération (max: 60s) |

### ⏩ Vitesse de Simulation

| Touche | Fonction | Valeur | Description |
|--------|----------|--------|-------------|
| **Q** | Ralentir | -1.0x | Réduit le fast-forward (min: 1.0x = vitesse normale) |
| **W** | Accélérer | +1.0x | Augmente le fast-forward (max: 100.0x) |

---

## 📊 Interface Utilisateur

### Panneau de Contrôles (Haut Droit)

Affichage permanent de :
- **État des toggles** (D, L, V)
- **Mutation Rate actuel** (ex: 0.15)
- **Generation Time actuel** (ex: 30s)
- **Fast Forward Rate actuel** (ex: 1.0x)
- **État du Debug Monitor**
- **Note sur terrain** (actuellement désactivé)

### Statistiques Principales (Haut Gauche)

Affichage en temps réel de :
- **Génération** : Numéro de la génération actuelle
- **Proies** : Nombre de proies vivantes (Gen X)
- **Prédateurs** : Nombre de prédateurs vivants (Gen X)
- **Nourriture** : Quantité de nourriture disponible
- **Temps** : Progression vers la prochaine génération
- **Fitness Moyenne** : Proies et prédateurs
- **Énergie Moyenne** : Proies
- **Captures** : Nombre de proies capturées
- **Faim Moyenne** : Temps depuis dernier repas des prédateurs

### Graphique de Fitness (Bas Gauche)

- **Courbe verte** : Fitness moyenne des proies
- **Courbe rouge** : Fitness moyenne des prédateurs
- **Historique** : 100 derniers points de mesure
- **Mise à jour** : Toutes les 0.3 secondes

### Debug Monitor (F1 - Bas Gauche)

Affiche en temps réel :
- **mutationRate** : Valeur actuelle du taux de mutation
- **generationTime** : Durée actuelle d'une génération
- **fastForwardRate** : Multiplicateur de vitesse actuel
- **frameCount** : Nombre de frames écoulées
- **mutation_change** : Dernier changement de mutation (détection de cycling)
- **genTime_change** : Dernier changement de genTime (détection de cycling)
- **ff_change** : Dernier changement de fast-forward (détection de cycling)

**Utilité** : Détecter les bugs de "cycling" où les valeurs changeraient sans input utilisateur.

---

## 🎯 Affichages Visuels Détaillés

### Cercles de Détection (D)

Lorsque activés, affichent :
- **Proies** : Cercle vert translucide (rayon: 80 pixels)
  - Détecte les prédateurs et la nourriture
  - Plus visible en cas de danger proche

- **Prédateurs** : Cercle rouge translucide (rayon: 100 pixels)
  - Détecte les proies à chasser
  - Rayon plus grand que les proies (avantage du chasseur)

**Utilité** : Comprendre pourquoi une créature réagit ou ne réagit pas.

### Lignes de Direction (L)

- **Blanc** : Ligne partant du centre de la créature
- **Longueur** : Proportionnelle à la vitesse (vel × 2.0)
- **Direction** : Indique vers où la créature se déplace

**Utilité** : Visualiser les trajectoires et prédire les mouvements.

### Vitesses Numériques (V)

- **Position** : Au-dessus de chaque créature
- **Format** : Magnitude de la vitesse (ex: "45.3")
- **Calcul** : `sqrt(vel.x² + vel.y²)`
- **Couleur** : Blanc

**Utilité** : Débugger les problèmes de vitesse, vérifier que les entités ne sont pas trop rapides/lentes.

---

## 🔧 Paramètres Détaillés

### Mutation Rate (Taux de Mutation)

**Plage** : 0.05 à 0.50
**Défaut** : 0.15
**Incrément** : 0.05

**Impact** :
- **Faible (0.05-0.10)** : Évolution lente, stratégies stables
- **Moyen (0.15-0.25)** : Bon équilibre exploration/exploitation
- **Élevé (0.30-0.50)** : Exploration agressive, instabilité possible

**Comment ça marche** :
```cpp
Pour chaque poids du réseau neuronal :
    if (random() < mutationRate) {
        poids += randomWeight() × 0.5
    }
```

**Conseil** :
- Démarrer avec 0.15
- Augmenter si la simulation stagne
- Réduire si les comportements sont trop chaotiques

### Generation Time (Temps de Génération)

**Plage** : 10s à 60s
**Défaut** : 30s
**Incrément** : 5s

**Impact** :
- **Court (10-20s)** : Évolution rapide, moins de temps pour apprendre
- **Moyen (25-35s)** : Bon équilibre
- **Long (40-60s)** : Plus de temps pour tester les stratégies, évolution lente

**Conseil** :
- **10s** : Tester rapidement des paramètres
- **30s** : Observation normale
- **60s** : Analyse approfondie d'une génération

### Fast Forward Rate (Accélération)

**Plage** : 1.0x à 100.0x
**Défaut** : 1.0x
**Incrément** : 1.0x

**Impact** :
- **1.0x** : Vitesse normale (60 FPS)
- **5.0x** : 5 fois plus rapide
- **20.0x** : Génération de 30s → 1.5s réelles
- **100.0x** : Maximum, utile pour skip rapidement

**Fonctionnement** :
```cpp
dt = clock.restart().asSeconds();
dt *= gui.fastForwardRate;  // Le temps est multiplié
sim.update(dt);              // Tout s'accélère proportionnellement
```

**Attention** : À très haute vitesse (>50x), l'affichage peut devenir saccadé mais la simulation reste correcte.

**Conseil** :
- **1-2x** : Observation détaillée
- **5-10x** : Passer rapidement les phases ennuyeuses
- **20-50x** : Tester sur de nombreuses générations
- **100x** : Skip jusqu'à un point d'intérêt

---

## 🐛 Utilisation du Debug Monitor

### Activation
Appuyez sur **F1** pour ouvrir/fermer le panneau.

### Que Surveiller

#### Comportement Normal
```
mutationRate: 0.150
generationTime: 30.000
fastForwardRate: 1.000
frameCount: 1234.000
mutation_change: 0.000
genTime_change: 0.000
ff_change: 0.000
```

Tous les `*_change` doivent être à **0.000** sauf quand vous appuyez sur une touche.

#### Bug Détecté (Cycling)
```
mutationRate: 0.200
generationTime: 30.000
fastForwardRate: 1.000
frameCount: 1235.000
mutation_change: 0.050  ← PROBLÈME !
genTime_change: 0.000
ff_change: 0.000
```

Si `mutation_change` est non-nul **sans avoir appuyé sur ←/→**, il y a un bug !

### Interprétation

| Valeur | Signification |
|--------|---------------|
| `mutation_change > 0` | Mutation rate a augmenté (bug ou touche →) |
| `mutation_change < 0` | Mutation rate a diminué (bug ou touche ←) |
| `genTime_change != 0` | Generation time a changé (bug ou touches ↑↓) |
| `ff_change != 0` | Fast forward a changé (bug ou touches Q/W) |

---

## 💡 Astuces et Workflows

### Workflow 1 : Démarrage Rapide
1. Lancer BioSim
2. Appuyer sur **W** plusieurs fois → Fast forward à 10-20x
3. Observer les premières générations évoluer rapidement
4. Réduire à 1x quand un comportement intéressant émerge

### Workflow 2 : Analyse Détaillée
1. Fast forward 1x (normal)
2. Activer **D** (détection) et **L** (direction)
3. Suivre une créature spécifique
4. Observer comment elle réagit aux stimuli

### Workflow 3 : Debugging
1. Appuyer sur **F1** pour ouvrir le Debug Monitor
2. Modifier un paramètre (ex: → pour mutation)
3. Vérifier que seul `mutation_change` est non-nul
4. Observer que le changement persiste (pas de cycling)

### Workflow 4 : Expérimentation Rapide
1. **W** jusqu'à 50x
2. **↓↓↓** pour réduire genTime à 15s
3. **→→** pour mutation à 0.25
4. Observer l'évolution sur 50+ générations en quelques minutes

### Workflow 5 : Comparaison de Stratégies
1. Lancer simulation A : mutation 0.10, genTime 30s
2. Noter les résultats après 20 générations
3. Relancer simulation B : mutation 0.25, genTime 30s
4. Comparer quelle stratégie évolue mieux

---

## 🎨 Légende Visuelle

### Couleurs des Créatures
- 🟢 **Vert** : Proie
- 🔴 **Rouge** : Prédateur
- 🟡 **Jaune** : Nourriture

### Cercles de Détection
- 🟢 **Vert translucide** : Zone de détection des proies
- 🔴 **Rouge translucide** : Zone de détection des prédateurs

### Graphique de Fitness
- 🟢 **Courbe verte** : Performance des proies
- 🔴 **Courbe rouge** : Performance des prédateurs

### Debug Monitor
- 🔵 **Panneau cyan** : Mode debug actif
- ⚪ **Texte blanc** : Valeurs normales
- 🔴 **Valeur non-nulle** : Changement détecté

---

## ⚠️ Limitations et Notes

### Limitations Connues
- **Fast Forward > 50x** : L'affichage peut être saccadé (normal)
- **Nombreuses créatures** : Performance peut diminuer (>100 entités)
- **Debug Monitor** : Prend de l'espace à l'écran

### Valeurs Limites
- Mutation Rate : Bloqué à [0.05, 0.50]
- Generation Time : Bloqué à [10s, 60s]
- Fast Forward : Bloqué à [1.0x, 100.0x]

### Comportements par Défaut
- Tous les toggles sont **activés** au démarrage (sauf vitesses)
- Debug Monitor est **désactivé** au démarrage
- Fast Forward commence à **1.0x** (vitesse normale)

---

## 🔍 FAQ Contrôles

### Q: Pourquoi mes touches ne marchent pas ?
**R**: Assurez-vous que la fenêtre SFML a le focus (cliquez dessus).

### Q: Comment réinitialiser les paramètres ?
**R**: Relancez le programme. Les valeurs par défaut sont toujours restaurées.

### Q: Puis-je modifier les contrôles ?
**R**: Oui, dans `src/gui.cpp`, fonction `handleInput()`.

### Q: Le Debug Monitor ralentit-il la simulation ?
**R**: Impact négligeable (<1% de performance).

### Q: Quelle est la touche pour pause ?
**R**: Pas de pause intégrée. Utilisez **Q** pour fast forward 1x, ou fermez la fenêtre.

### Q: Comment sauvegarder mon état ?
**R**: Pas encore implémenté. Feature future.

---

**Dernière mise à jour** : 04/11/2025
**Maintenu par** : megaray

Pour des questions, voir [GitHub Issues](https://github.com/megaray/BioSim/issues)
