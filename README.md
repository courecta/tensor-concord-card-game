# Tensor::Concord

**Tensor::Concord** is a strategic trading card game (TCG) where players harness the power of **tensors** to summon champions, activate artifacts, and battle for dominance in the realm of **Tensoria**. Designed for both casual and competitive players, Tensor::Concord combines deep strategy with engaging gameplay mechanics.

---

## **Project Overview**

This project is being developed in **C++** using the **SFML library** for 2D graphics and input handling. The game is currently in its early development phase and aims to explore foundational game mechanics, modular design, and scalability for future expansions.

### **Current Goals**
- Create a playable prototype with:
  - Core card mechanics (deck management, card effects).
  - Turn-based gameplay (player actions and responses).
  - Basic visual representation of cards and the battlefield using SFML.

---

## **Game Theme**

The game is set in the mystical realm of **Tensoria**, where **Tensor Weavers** battle to achieve **Concord**—a harmonious balance of power—or to dominate the battlefield with sheer strength. Players wield **champions**, **artifacts**, and **tensors**, choosing to play aggressively, defensively, or strategically to outwit their opponents.

---

## **Game Mechanics**

### **Core Concepts**
1. **Card Types**:
   - **Champions**: Units with unique stats and abilities that fight on the battlefield.
   - **Artifacts**: Support cards that enhance champions, disrupt opponents, or alter game states.
   - **Tensors**: Resources that fuel abilities, summon champions, or activate artifacts.

2. **Resource Management**:
   - **Tensor Energy**: A regenerating resource that players allocate strategically each turn.

3. **Victory Conditions**:
   - **Harmony Win**: Achieve control of three **Concord Nodes** (key positions on the battlefield) for two consecutive turns.
   - **Domination Win**: Destroy your opponent’s **Tensor Core**, depleting their energy reserves.

---

### **Game Phases**
Each turn is divided into the following phases:
1. **Draw Phase**:
   - Players draw two cards from their deck at the start of their turn.

2. **Tensor Allocation Phase**:
   - Players gain Tensor Energy and allocate it to summoning champions, activating artifacts, or powering abilities.

3. **Action Phase**:
   - Players can:
     - Place champions onto the battlefield.
     - Play artifacts or activate card effects.
     - Attack the opponent’s champions, Tensor Core, or Concord Nodes.

4. **End Phase**:
   - Players reset abilities and prepare for their opponent’s turn.

---

## **Planned Features**
- **Interactive Board**:
  - A grid-based battlefield (e.g., 3x3) where champions are placed, and positional strategy plays a role.
- **Dynamic Card Effects**:
  - Abilities that trigger upon specific conditions (e.g., when a card is summoned, damaged, or destroyed).
- **Customizable Decks**:
  - Players will build their decks from a pool of cards with varying playstyles (aggressive, control, etc.).
- **Simple AI**:
  - Opponent logic for single-player games.

---

## **Development Status**

### **Current Progress**
- **Dependencies Installed**: SFML and development environment set up on Linux.
- **Basic Functionality**:
  - Window rendering and simple shapes (e.g., placeholder cards).
  - Basic game loop implementation (input handling and updates).

### **Next Steps**
1. Implement core classes:
   - `Card`: Represents individual cards (champions, artifacts).
   - `Deck`: Manages the player’s card pool and shuffling.
   - `Game`: Oversees the game loop, turn order, and win conditions.
2. Display a 2D grid to represent the battlefield.
3. Render cards dynamically on the grid with basic interaction.

---

## **How to Build and Run**

### **Requirements**
- **C++ Compiler**: GCC or Clang (with C++17 support).
- **SFML Library**: Ensure SFML is installed and linked correctly.

### **Build Instructions**
1. Clone the repository:
   ```bash
   git clone https://github.com/courecta/tensor-concord-card-game
   cd tensor-concord-card-game
   ```
2. Build the game using the Makefile:
   ```bash
   make
   ```
3. Run the game:
   ```bash
   ./bin/TensorConcord
   ```

### **Clean the Build**
To remove compiled files and binaries:
```bash
make clean
```

---

## **Credits**
- **Game Design & Development**: *Courecta*  

---

## **Contribution Guidelines**
This is a learning project, as such, it may underperform, but I won't be really investing into it as game but more as an experience for game development.

---