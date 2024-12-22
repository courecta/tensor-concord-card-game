#include "game.h"
#include <sstream>

// Card static methods implementation
Card Card::createChampion(const std::string& name, int cost, int attack, int health) {
    return Card{CHAMPION, name, cost, attack, health, 0};
}

Card Card::createTensor(const std::string& name, int cost, int energyAmount) {
    return Card{TENSOR, name, cost, 0, 0, energyAmount};
}

// GameState methods implementation
void GameState::initializeDeck() {
    deck.clear();
    deck.push_back(Card::createChampion("Warrior", 1, 2, 3));
    deck.push_back(Card::createTensor("Energy", 0, 1));
    deck.push_back(Card::createChampion("Mage", 2, 3, 2));
    deck.push_back(Card::createTensor("Power", 0, 2));
}

void GameState::printGameState() const {
    // Clear screen using ANSI escape code
    std::cout << "\033[2J\033[H";
    
    // Print enemy info with ASCII border
    std::cout << "╔════════════ ENEMY ═══════════╗\n";
    std::cout << "║ Health: " << enemyHealth << "    Energy: " << enemyEnergy << " ║\n";
    std::cout << "╚═══════════════════════════════╝\n\n";

    // Print enemy field
    std::cout << "Enemy Field:\n";
    if (enemyField.empty()) {
        std::cout << "┌────────┐  (empty)\n";
        std::cout << "└────────┘\n";
    } else {
        for (const auto& card : enemyField) {
            std::cout << "┌────────┐\n";
            std::cout << "│" << card.name.substr(0, 8) << std::string(8 - std::min(8ul, card.name.length()), ' ') << "│\n";
            std::cout << "│ATK:" << card.attack << "   │\n";
            std::cout << "│HP:" << card.health << "    │\n";
            std::cout << "└────────┘  ";
        }
        std::cout << "\n";
    }

    // Print battlefield separator
    std::cout << "\n════════════ BATTLEFIELD ════════════\n\n";

    // Print player field
    std::cout << "Your Field:\n";
    if (playerField.empty()) {
        std::cout << "┌────────┐  (empty)\n";
        std::cout << "└────────┘\n";
    } else {
        for (size_t i = 0; i < playerField.size(); i++) {
            const auto& card = playerField[i];
            std::cout << "┌────────┐\n";
            std::cout << "│" << card.name.substr(0, 8) << std::string(8 - std::min(8ul, card.name.length()), ' ') << "│ [" << i << "]\n";
            std::cout << "│ATK:" << card.attack << "   │\n";
            std::cout << "│HP:" << card.health << "    │\n";
            std::cout << "└────────┘  ";
        }
        std::cout << "\n";
    }

    // Print player hand with better formatting
    std::cout << "\n╔═══════════════════ YOUR HAND ═════════════════╗\n";
    if (playerHand.empty()) {
        std::cout << "║               (Hand is empty)                ║\n";
    } else {
        for (size_t i = 0; i < playerHand.size(); i++) {
            const auto& card = playerHand[i];
            std::cout << "║ [" << i << "] ";
            // Card name with padding
            std::cout << std::left << std::setw(12) << card.name << " | ";
            
            // Card stats based on type
            if (card.type == Card::CHAMPION) {
                std::cout << "Cost:" << std::setw(2) << card.cost 
                         << " ATK:" << std::setw(2) << card.attack 
                         << " HP:" << std::setw(2) << card.health;
            } else if (card.type == Card::TENSOR) {
                std::cout << "Cost:" << std::setw(2) << card.cost 
                         << " Energy+" << std::setw(2) << card.effect 
                         << "    ";
            }
            std::cout << " ║\n";
        }
    }
    std::cout << "╚════════════════════════════════════════════╝\n\n";

    // Print player info
    std::cout << "╔════════════ YOU ══════════════╗\n";
    std::cout << "║ Health: " << playerHealth << "    Energy: " << playerEnergy << " ║\n";
    std::cout << "╚═══════════════════════════════╝\n\n";
}

// Game class implementation
Game::Game() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    setupGame();
}

void Game::setupGame() {
    state.initializeDeck();
    state.playerEnergy = 1;
    state.enemyEnergy = 1;
    
    // Draw initial hands
    for (int i = 0; i < 5; i++) {
        drawCard(state.playerHand);
        drawCard(state.enemyHand);
    }
}

void Game::run() {
    std::cout << "Game started!\n";
    printHelp();
    
    while (!gameOver()) {
        state.printGameState();
        
        if (state.isPlayerTurn) {
            std::cout << "\nEnter command (or 'help'): ";
            std::string command;
            std::getline(std::cin, command);
            handleCommand(command);
        } else {
            performEnemyTurn();
        }
    }
}

void Game::handleCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if (action == "help") {
        printHelp();
    }
    else if (action == "play") {
        int cardIndex;
        if (iss >> cardIndex) {
            playCardFromHand(cardIndex);
        } else {
            std::cout << "Usage: play <card_index>\n";
        }
    }
    else if (action == "attack") {
        int cardIndex;
        if (iss >> cardIndex) {
            attackWithCard(cardIndex);
        } else {
            std::cout << "Usage: attack <card_index>\n";
        }
    }
    else if (action == "end") {
        endPlayerTurn();
    }
    else if (action == "quit") {
        state.playerHealth = 0;
    }
    else {
        std::cout << "Unknown command. Type 'help' for commands.\n";
    }
}

void Game::drawCard(std::vector<Card>& hand) {
    if (!state.deck.empty()) {
        hand.push_back(state.deck.back());
        state.deck.pop_back();
    }
}

void Game::playCardFromHand(int cardIndex) {
    if (cardIndex < 0 || cardIndex >= static_cast<int>(state.playerHand.size())) {
        std::cout << "Invalid card index\n";
        return;
    }

    Card& card = state.playerHand[cardIndex];
    if (card.cost > state.playerEnergy) {
        std::cout << "Not enough energy!\n";
        return;
    }

    switch (card.type) {
        case Card::CHAMPION:
            if (state.playerField.size() < 5) {
                state.playerEnergy -= card.cost;
                state.playerField.push_back(card);
                state.playerHand.erase(state.playerHand.begin() + cardIndex);
                std::cout << "Played " << card.name << " to field\n";
            } else {
                std::cout << "Field is full!\n";
            }
            break;

        case Card::TENSOR:
            state.playerEnergy += card.effect;
            state.playerHand.erase(state.playerHand.begin() + cardIndex);
            std::cout << "Used " << card.name << " for " << card.effect << " energy\n";
            break;
    }
}

void Game::attackWithCard(int cardIndex) {
    if (cardIndex < 0 || cardIndex >= static_cast<int>(state.playerField.size())) {
        std::cout << "Invalid card index\n";
        return;
    }

    Card& attacker = state.playerField[cardIndex];
    state.enemyHealth -= attacker.attack;
    std::cout << attacker.name << " attacks enemy for " << attacker.attack << " damage!\n";
}

void Game::endPlayerTurn() {
    state.isPlayerTurn = false;
    state.playerEnergy += 1;
    drawCard(state.playerHand);
    std::cout << "\nEnding your turn...\n";
    performEnemyTurn();
}

void Game::performEnemyTurn() {
    std::cout << "\nEnemy turn...\n";
    
    // Simple AI: Try to play any card it can
    for (size_t i = 0; i < state.enemyHand.size(); i++) {
        if (state.enemyHand[i].cost <= state.enemyEnergy) {
            playEnemyCard(i);
            break;
        }
    }

    // Attack with all field cards
    for (const auto& champion : state.enemyField) {
        state.playerHealth -= champion.attack;
        std::cout << "Enemy " << champion.name << " attacks for " 
                 << champion.attack << " damage!\n";
    }

    state.isPlayerTurn = true;
    state.enemyEnergy += 1;
    drawCard(state.enemyHand);
    std::cout << "Enemy turn ended.\n\n";
}

void Game::playEnemyCard(int cardIndex) {
    Card& card = state.enemyHand[cardIndex];
    
    switch (card.type) {
        case Card::CHAMPION:
            if (state.enemyField.size() < 5) {
                state.enemyEnergy -= card.cost;
                state.enemyField.push_back(card);
                state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
                std::cout << "Enemy plays " << card.name << "\n";
            }
            break;
            
        case Card::TENSOR:
            state.enemyEnergy += card.effect;
            state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
            std::cout << "Enemy uses " << card.name << "\n";
            break;
    }
}

void Game::printHelp() const {
    std::cout << "\nCommands:\n"
              << "  play <index>   - Play card from hand\n"
              << "  attack <index> - Attack with card on field\n"
              << "  end           - End your turn\n"
              << "  help          - Show this help\n"
              << "  quit          - Exit game\n\n";
}

bool Game::gameOver() const {
    if (state.playerHealth <= 0) {
        std::cout << "\nGame Over - You lost!\n";
        return true;
    }
    if (state.enemyHealth <= 0) {
        std::cout << "\nCongratulations - You won!\n";
        return true;
    }
    return false;
}
