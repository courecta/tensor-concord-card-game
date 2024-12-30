#include "game.h"

const int GameState::TensorState::RAINBOW_COLORS[7] = {
    COLOR_RED, COLOR_YELLOW, COLOR_GREEN,
    COLOR_CYAN, COLOR_BLUE, COLOR_MAGENTA, COLOR_WHITE
};

void GameState::initializeDeck() {
    deck.clear();
    
    const char* championNames[] = {
        "Netrunner", "Cybermage", "Datascraper", "GridKnight",
        "ByteBlade", "SyncMaster", "CodeWeaver", "VoidHacker"
    };
    
    const char* artifactNames[] = {
        "Neural Link", "BioAugment", "DataCore", "SynapseBoost",
        "TechPlating", "QuickChips", "PowerNode", "GridAmp"
    };
    
    const char* tensorNames[] = {
        "Data Shard", "Energy Core", "Power Node", "Logic Gate", "Sync Crystal"
    };

    const char* roleNames[4][8] = {
        // MERC names
        {"Bounty", "Hunter", "Merc", "Gunner", "Soldier", "Warrior", "Fighter", "Sniper"},
        // NOMAD names
        {"Wanderer", "Drifter", "Ranger", "Scout", "Tracker", "Pathfinder", "Guide", "Explorer"},
        // CORPO names
        {"Executive", "Manager", "Director", "Leader", "Chief", "Boss", "Head", "Commander"},
        // MAGE names
        {"Wizard", "Sorcerer", "Mage", "Caster", "Mystic", "Sage", "Scholar", "Adept"}
    };

    // Create champions with role-appropriate names
    for (int i = 0; i < 37; i++) {
        int roleIdx = (i / 4) % 4;
        Card champion = Card::createChampion(
            roleNames[roleIdx][i % 8],
            1 + (i % 3),      // cost
            1 + (i % 3),      // attack
            2 + (i % 2)       // health
        );
        champion.faction = Card::Faction(1 + (i % 4));  // TECHNO to VIRTU_MACHINA
        champion.role = Card::Role(1 + ((i / 4) % 4));  // MERC to MAGE
        deck.push_back(champion);
    }

    // Create artifacts with themed names
    for (int i = 0; i < 8; i++) {
        int buff = (i % 2 == 0) ? 1 : 2;
        deck.push_back(Card::createArtifact(artifactNames[i], buff, buff));
    }

    // Create tensors with themed names
    for (int i = 0; i < 5; i++) {
        int energyBoost = (i % 2 == 0) ? 1 : 2;
        deck.push_back(Card::createTensor(tensorNames[i], 0, energyBoost));
    }

    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device{}()));
    tensor.current = 0;
    tensor.maximum = 3;
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

    // Draw tensor gauge with rainbow colors
    GameUI::drawTensorGauge(LINES/2-3, infoX+1, tensor.current, tensor.maximum);
    
    // Show maximum value
    std::string maxInfo = "Max: " + std::to_string(tensor.maximum);
    if (tensor.maximum < tensor.ABSOLUTE_MAX) {
        maxInfo += " (Next: " + std::to_string(tensor.maximum + 1) + ")";
    }
    mvprintw(LINES/2-2, infoX+1, "%s", maxInfo.c_str());

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
        endwin();  // Clean up ncurses
        exit(0);   // Exit program
    }
    if (state.enemyHealth <= 0) {
        clear();
        mvprintw(LINES/2, (COLS-40)/2, "Congratulations - You have achieved Concord!");
        refresh();
        napms(2000);
        endwin();  // Clean up ncurses
        exit(0);   // Exit program
    }
    return false;
}