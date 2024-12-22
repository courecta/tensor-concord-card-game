#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>

// Forward declarations
struct Card;
struct GameState;
class Game;

// Card structure
struct Card {
    enum Type { 
        CHAMPION,
        ARTIFACT,
        TENSOR
    } type;

    std::string name;
    int cost;
    int attack;
    int health;
    int effect;

    static Card createChampion(const std::string& name, int cost, int attack, int health);
    static Card createTensor(const std::string& name, int cost, int energyAmount);
};

// Game state
struct GameState {
    int playerHealth = 10;
    int enemyHealth = 10;
    int playerEnergy = 1;
    int enemyEnergy = 1;
    bool isPlayerTurn = true;
    
    std::vector<Card> deck;
    std::vector<Card> playerHand;
    std::vector<Card> enemyHand;
    std::vector<Card> playerField;
    std::vector<Card> enemyField;
    
    void initializeDeck();
    void printGameState() const;
};

// Main game class
class Game {
private:
    GameState state;
    std::mt19937 rng;

public:
    Game();
    void run();

private:
    void setupGame();
    void drawCard(std::vector<Card>& hand);
    void handleCommand(const std::string& cmd);
    void playCardFromHand(int cardIndex);
    void attackWithCard(int cardIndex);
    void endPlayerTurn();
    void performEnemyTurn();
    void playEnemyCard(int cardIndex);
    void printHelp() const;
    bool gameOver() const;
};
