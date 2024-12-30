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
#include <map>
#include <array>
#include <set>
// Using PDCurses on windows, please follow README.md for instructions if compilation does not work
#include <curses.h>
// #include "gameui.h"  // do NOT make this header, it will break things...

struct Card;
struct GameState;
class Game;
class GameUI;

struct Card
{
    enum Type
    {
        CHAMPION,
        ARTIFACT,
        TENSOR
    } type;

    enum Faction {
        FACTION_NONE,    // Changed from NONE to FACTION_NONE
        TECHNO,      // ATK buff
        CYBER,       // HP buff
        EXEC,        // Cost reduction
        VIRTU_MACHINA // Energy + Tensor
    } faction = FACTION_NONE;

    enum Role {
        ROLE_NONE,      // Changed from NONE to ROLE_NONE
        MERC,
        NOMAD,
        CORPO,
        MAGE
    } role = ROLE_NONE;

    std::string name;
    int cost;
    int attack;
    int health;
    int effect;
    int turnsInPlay = 0;
    bool hasAttackedThisTurn = false;
    int originalAttack;  // Add these new fields
    int originalHealth;
    bool hasSynergyBuff = false;

    // Add constructors with default values
    Card(Type t, const std::string& n, int c, int atk, int hp, int eff) :
        type(t), name(n), cost(c), attack(atk), health(hp), effect(eff),
        originalAttack(atk), originalHealth(hp),  // Initialize original stats
        faction(FACTION_NONE), role(ROLE_NONE), turnsInPlay(0), 
        hasAttackedThisTurn(false), hasSynergyBuff(false) {}

    // Factory methods now use the constructor
    static Card createChampion(const std::string& name, int cost, int attack, int health) {
        return Card(CHAMPION, name, cost, attack, health, 0);
    }

    static Card createTensor(const std::string& name, int cost, int energyAmount) {
        return Card(TENSOR, name, cost, 0, 0, energyAmount);
    }

    static Card createArtifact(const std::string& name, int cost, int effectValue) {
        return Card(ARTIFACT, name, cost, 0, 0, effectValue);
    }
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

    struct TensorState {
        int current = 0;
        int maximum = 3;
        static const int ABSOLUTE_MAX = 7;
        static const int RAINBOW_COLORS[7];
    } tensor;
    
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

    // Add minigame methods
    bool playCoinToss();
    bool playHighLow();
    bool playRoulette();
    bool playDiceRoll();
    bool playRPS();
    void applyMinigameConsequence(bool won);

    // Update synergy method calls to include hand parameter
    void checkAndApplySynergies(std::vector<Card>& field, std::vector<Card>& hand);
    int calculateSynergyLevel(const std::vector<Card>& field, Card::Faction faction);
    void applySynergyEffects(std::vector<Card>& field, std::vector<Card>& hand, Card::Faction faction, int level);

    // Add tensor-related checks to these methods
    void increaseTensorGauge(int amount);
    void handleTensorPeak();
    void resetTensorGauge();

    void playMainGame();
    void playMinigameMenu();

    // Consolidate menu handling into one method
    int showMenu(const std::vector<std::string>& options, const std::string& title = "") {
        int choice = 0;
        const int MENU_WIDTH = 30; // Move constant here instead of using GameUI::MENU_WIDTH
        
        while(true) {
            clear();
            box(stdscr, 0, 0);
            
            if(!title.empty()) {
                std::string titleDecor = "=== " + title + " ===";
                mvprintw(2, (COLS-titleDecor.length())/2, "%s", titleDecor.c_str());
            }
            
            int startY = (LINES - options.size()) / 2;
            for(size_t i = 0; i < options.size(); i++) {
                int x = (COLS - MENU_WIDTH) / 2;
                if(i == choice) {
                    attron(A_REVERSE);
                    mvprintw(startY + i, x, "[%*s]", MENU_WIDTH-2, options[i].c_str());
                    attroff(A_REVERSE);
                } else {
                    mvprintw(startY + i, x, "[ %-*s ]", MENU_WIDTH-4, options[i].c_str());
                }
            }
            
            mvprintw(LINES-2, (COLS-35)/2, "UP/DOWN to select, ENTER to confirm");
            refresh();

            switch(getch()) {
                case KEY_UP:
                    choice = (choice > 0) ? choice - 1 : options.size() - 1;
                    break;
                case KEY_DOWN:
                    choice = (choice < options.size() - 1) ? choice + 1 : 0;
                    break;
                case '\n':
                    return choice;
                case 27:  // ESC
                    return -1;
            }
        }
    }
};

// GameUI.h copied over for simplicity also so to avoid including each other

class GameUI {
public:
    // Layout constants
    static constexpr int CARD_WIDTH = 18;      // Base card width
    static constexpr int CARD_HEIGHT = 5;      // Base card height
    static constexpr int CARD_SPACING = 20;    // Universal spacing between cards
    static constexpr int MENU_WIDTH = 30;      // Width for menu boxes
    static constexpr int MENU_PADDING = 2;     // Padding for menu items

    // Menu actions
    static constexpr int ACTION_PLAY = 0;
    static constexpr int ACTION_ATTACK = 1;
    static constexpr int ACTION_END = 2;
    static constexpr int ACTION_HELP = 3;
    static constexpr int ACTION_QUIT = 4;

    // Color constants
    static constexpr int TENSOR_COLOR_START = 1;
    static constexpr int FACTION_COLOR_START = 8;
    static constexpr int BUFF_COLOR = 12;
    static constexpr int TOTAL_COLORS = 13;

    // Drawing methods
    static void drawBox(int y, int x, int height, int width);
    static void drawCard(WINDOW* win, int y, int x, const Card& card, bool isSelected = false);
    static void drawField(WINDOW* win, int y, int x, const std::vector<Card>& field, int selectedIndex = -1);
    static void drawHand(WINDOW* win, int y, int x, const std::vector<Card>& hand, int selectedIndex = -1);
    static void drawStats(WINDOW* win, int y, int x, int health, int energy);
    static void drawHealthBar(int y, int x, int current, int max, bool isEnergy = false);
    static void drawTensorGauge(int y, int x, int current, int maximum);

    // Menu methods
    static void drawActionMenu(int selectedAction);
    static void drawStatusBar(const std::string& status);
    static void drawNavigationHints();
    static void drawCenteredMenu(const std::vector<std::string>& options, int selected);
    static void highlightSelectedCard(int y, int x, int width, int height);

    // Color methods
    static int getFactionColor(Card::Faction faction);
    static void initializeAllColors();
    static void initializeTensorColors();
};

constexpr size_t MAX_FIELD_SIZE = 4;