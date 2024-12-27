#pragma once
#include <curses.h>
#include <string>
#include <vector>

// Forward declare needed types
struct Card;

#include "game.h"

class GameUI {
public:
    static void drawBox(int y, int x, int height, int width);
    static void drawCard(WINDOW* win, int y, int x, const Card& card, bool isSelected = false);
    static void drawField(WINDOW* win, int y, int x, const std::vector<Card>& field, int selectedIndex = -1);
    static void drawHand(WINDOW* win, int y, int x, const std::vector<Card>& hand, int selectedIndex = -1);  // Updated signature
    static void drawStats(WINDOW* win, int y, int x, int health, int energy);
    
    // New methods for menu system
    static void drawActionMenu(int selectedAction);
    static void drawStatusBar(const std::string& status);
    static void drawNavigationHints();
    static void drawHealthBar(int y, int x, int current, int max, bool isEnergy = false);
    static void highlightSelectedCard(int y, int x, int width, int height);
    
    // Constants for menu actions
    static constexpr int ACTION_PLAY = 0;
    static constexpr int ACTION_ATTACK = 1;
    static constexpr int ACTION_END = 2;
    static constexpr int ACTION_HELP = 3;
    static constexpr int ACTION_QUIT = 4;
};
