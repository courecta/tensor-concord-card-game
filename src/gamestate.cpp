#include "game.h"
#include "gameui.h"  // Add this line

void GameState::initializeDeck() {
    deck.clear();
    // 50 cards: 75% champion (37/38), 15% artifact (~7/8), 10% tensor (5).
    // For simplicity, 37 champion, 8 artifact, 5 tensor:
    for (int i = 0; i < 37; i++) {
        deck.push_back(Card::createChampion("Champion", 1 + (i % 3), 1 + (i % 3), 2 + (i % 2)));
    }
    for (int i = 0; i < 8; i++) {
        // +1 or +2 effect
        int buff = (i % 2 == 0) ? 1 : 2;
        deck.push_back(Card::createArtifact("Artifact", buff, buff));
    }
    for (int i = 0; i < 5; i++) {
        int energyBoost = (i % 2 == 0) ? 1 : 2;
        deck.push_back(Card::createTensor("Tensor", 0, energyBoost));
    }
    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device{}()));
}

void GameState::printGameState() const {
    clear();
    box(stdscr, 0, 0);

    // Main game area (left side)
    int gameWidth = COLS - 30;  // Reserve 30 columns for info panel
    
    // Title
    mvprintw(1, (gameWidth-16)/2, "Tensor Concord");
    mvhline(2, 1, ACS_HLINE, gameWidth-2);

    // Enemy section
    GameUI::drawHealthBar(4, 2, enemyHealth, 10);
    GameUI::drawHealthBar(5, 2, enemyEnergy, 10, true);
    GameUI::drawField(stdscr, 7, 2, enemyField);

    // Info panel with better organization
    int infoX = gameWidth + 1;
    mvvline(1, infoX-1, ACS_VLINE, LINES-2);

    // Enemy section in panel
    attron(A_BOLD);
    mvprintw(3, infoX+1, "Enemy Status");
    attroff(A_BOLD);
    mvprintw(4, infoX+1, "Hand: %zu", enemyHand.size());
    mvprintw(5, infoX+1, "Cards Left: %zu", deck.size());
    
    // Center section can show current phase/status
    mvprintw(LINES/2-1, infoX+1, "Phase: %s", 
             isPlayerTurn ? "Your Turn" : "Enemy Turn");

    // Player section in panel
    attron(A_BOLD);
    mvprintw(LINES/2+1, infoX+1, "Your Status");
    attroff(A_BOLD);
    GameUI::drawHealthBar(LINES/2+3, infoX+1, playerHealth, 10);
    GameUI::drawHealthBar(LINES/2+4, infoX+1, playerEnergy, 10, true);
    mvprintw(LINES/2+5, infoX+1, "Hand: %zu/%zu", 
             playerHand.size(), playerHand.size() + deck.size());

    // Battlefield
    mvhline(LINES/2-1, 1, ACS_HLINE, gameWidth-2);
    attron(A_BOLD);
    mvprintw(LINES/2-1, (gameWidth-13)/2, " Battlefield ");
    attroff(A_BOLD);

    // Player section
    GameUI::drawField(stdscr, LINES/2+2, 2, playerField);
    GameUI::drawHand(stdscr, LINES-9, 2, playerHand);

    refresh();
}

bool Game::isGameOver() const {
    if (state.playerHealth <= 0) {
        clear();
        mvprintw(LINES/2, (COLS-40)/2, "Game Over - The enemy has achieved Concord!");
        refresh();
        napms(2000);
        return true;
    }
    if (state.enemyHealth <= 0) {
        clear();
        mvprintw(LINES/2, (COLS-40)/2, "Congratulations - You have achieved Concord!");
        refresh();
        napms(2000);
        return true;
    }
    return false;
}