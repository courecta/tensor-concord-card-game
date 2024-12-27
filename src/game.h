#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

// Using PDCurses on windows, please follow README.md for instructions if compilation does not work
#include <curses.h>
#include "gameui.h"  // Add this include

struct Card;
struct GameState;
class Game;
class GameUI;  // Add this line

struct Card
{
    enum Type
    {
        CHAMPION,
        ARTIFACT,
        TENSOR
    } type;

    std::string name;
    int cost;
    int attack;
    int health;
    int effect;
    int turnsInPlay = 0;
    bool hasAttackedThisTurn = false;

    static Card createChampion(const std::string& name, int cost, int attack, int health);
    static Card createTensor(const std::string& name, int cost, int energyAmount);
    static Card createArtifact(const std::string& name, int cost, int effectValue);
};

struct GameState
{
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

class Game
{
private:
    GameState state;
    std::mt19937 rng;
    WINDOW* mainwin;  // Main window for the game

public:
    Game();
    ~Game();  // Add destructor to clean up PDCurses
    void run();

private:
    // New UI methods
    void initializeUI();
    void cleanupUI();
    void refreshUI();
    int getMenuChoice(const std::vector<std::string>& options, int startY);
    void displayMenu();
    void handleKeyboardInput();
    void initializeGame();
    void drawCard(std::vector<Card>& hand);
    void handleCommand(const std::string& cmd);
    void playCardFromHand(int cardIndex);
    void attackWithCard(int cardIndex);
    void endPlayerTurn();
    void performEnemyTurn();
    void playEnemyCard(int cardIndex);
    void printHelp() const;
    void showHelpMenu() const;
    bool isGameOver() const;
    void handleAction(int action);  // Add these new method declarations
    bool promptYesNo(const std::string& question);  // Add these new method declarations
};

constexpr size_t MAX_FIELD_SIZE = 4;