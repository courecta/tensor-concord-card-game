#include "game.h"

// Game class implementation
Game::Game() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    initializeUI();
    GameUI::initializeAllColors();  // Initialize all colors at once
    initializeGame();
}

Game::~Game() {
    cleanupUI();
}

void Game::initializeUI() {
    mainwin = stdscr;
    clear();
    refresh();
}

void Game::cleanupUI() {
    // Nothing needed here as main() handles endwin()
}

void Game::initializeGame()
{
    state.initializeDeck();
    state.playerEnergy = 1;
    state.enemyEnergy = 1;
    
    // Draw initial hands
    for(int i = 0; i < 5; i++)
    {
        drawCard(state.playerHand);
        drawCard(state.enemyHand);
    }
}

void Game::run() {
    std::vector<std::string> mainMenuOptions = {
        "Start Game",
        "Practice Minigames",
        "Quit"
    };
    
    bool running = true;
    while(running) {
        int choice = showMenu(mainMenuOptions, "TENSOR CONCORD");
        switch(choice) {
            case 0: playMainGame(); break;
            case 1: playMinigameMenu(); break;
            case 2: /* fallthrough */
            case -1: running = false; break;
        }
    }
}

// Rename the original run() to:
void Game::playMainGame() {
    int selectedAction = 0;
    bool running = true;
    
    clear();
    box(stdscr, 0, 0);
    mvprintw(LINES/2, (COLS-20)/2, "Welcome to Tensor Concord!");
    refresh();
    napms(2000);
    
    while(running && !isGameOver()) {
        state.printGameState();
        
        if(state.isPlayerTurn) {
            GameUI::drawActionMenu(selectedAction);
            GameUI::drawStatusBar("Your turn - Choose an action");
            GameUI::drawNavigationHints();
            refresh();

            int ch = getch();
            switch(ch) {
                case KEY_LEFT:
                    selectedAction = (selectedAction > 0) ? selectedAction - 1 : GameUI::ACTION_QUIT;
                    break;
                case KEY_RIGHT:
                    selectedAction = (selectedAction < GameUI::ACTION_QUIT) ? selectedAction + 1 : 0;
                    break;
                case '\n':
                    handleAction(selectedAction);
                    break;
                case 27: // ESC
                    if(promptYesNo("Quit game?")) {
                        running = false;
                    }
                    break;
            }
        } else {
            performEnemyTurn();
        }
    }
}

void Game::handleAction(int action) {
    switch(action) {
        case GameUI::ACTION_PLAY:
            if(!state.playerHand.empty()) {
                int selected = 0;
                const size_t cardsPerPage = 4;
                while(true) {
                    state.printGameState();
                    GameUI::drawHand(stdscr, LINES-8, 2, state.playerHand, selected);
                    
                    // Show card details at the bottom
                    const auto& selectedCard = state.playerHand[selected];
                    std::string details;
                    if (selectedCard.type == Card::CHAMPION) {
                        details = "Champion - Can attack enemy or enemy champions";
                    } else if (selectedCard.type == Card::ARTIFACT) {
                        details = "Artifact - Buffs champion's ATK or HP";
                    } else {
                        details = "Tensor - Provides immediate energy";
                    }
                    GameUI::drawStatusBar(details);
                    refresh();

                    int ch = getch();
                    switch(ch) {
                        case KEY_LEFT:
                            selected = (selected > 0) ? selected - 1 : state.playerHand.size() - 1;
                            break;
                        case KEY_RIGHT:
                            selected = (selected < state.playerHand.size() - 1) ? selected + 1 : 0;
                            break;
                        case '\n':
                            playCardFromHand(selected);
                            return;
                        case 27:
                            return;
                    }
                }
            }
            break;

        case GameUI::ACTION_ATTACK:
            if(!state.playerField.empty()) {
                // Check if there are any available attackers first
                bool hasAvailableAttackers = false;
                for(const auto& card : state.playerField) {
                    if(!card.hasAttackedThisTurn && card.turnsInPlay > 0) {
                        hasAvailableAttackers = true;
                        break;
                    }
                }

                if(!hasAvailableAttackers) {
                    GameUI::drawStatusBar("No champions available to attack!");
                    refresh();
                    napms(1500);
                    return;
                }

                int selected = 0;
                while(true) {
                    state.printGameState();
                    
                    // Highlight available attackers
                    for(size_t i = 0; i < state.playerField.size(); i++) {
                        const auto& card = state.playerField[i];
                        bool canAttack = !card.hasAttackedThisTurn && card.turnsInPlay > 0;
                        GameUI::drawCard(stdscr, LINES/2+2, 2 + (i * 10), card, i == selected && canAttack);
                    }

                    GameUI::drawStatusBar("Select attacker (Left/Right to choose, Enter to select, ESC to cancel)");
                    refresh();

                    int ch = getch();
                    switch(ch) {
                        case KEY_LEFT:
                            do {
                                selected = (selected > 0) ? selected - 1 : state.playerField.size() - 1;
                            } while(state.playerField[selected].hasAttackedThisTurn || 
                                   state.playerField[selected].turnsInPlay == 0);
                            break;
                        case KEY_RIGHT:
                            do {
                                selected = (selected < state.playerField.size() - 1) ? selected + 1 : 0;
                            } while(state.playerField[selected].hasAttackedThisTurn || 
                                   state.playerField[selected].turnsInPlay == 0);
                            break;
                        case '\n':
                            if(!state.playerField[selected].hasAttackedThisTurn && 
                               state.playerField[selected].turnsInPlay > 0) {
                                attackWithCard(selected);
                                return;
                            }
                            break;
                        case 27:
                            return;
                    }
                }
            }
            break;

        case GameUI::ACTION_END:
            endPlayerTurn();
            break;

        case GameUI::ACTION_HELP:
            showHelpMenu();
            break;

        case GameUI::ACTION_QUIT:
            if(promptYesNo("Quit game?")) {
                state.playerHealth = 0;
            }
            break;
    }
}

bool Game::promptYesNo(const std::string& question) {
    GameUI::drawStatusBar(question + " (Y/N)");
    refresh();
    
    while(true) {
        int ch = getch();
        if(tolower(ch) == 'y') {
            return true;
        } else if(tolower(ch) == 'n') {
            return false;
        }
    }
}

void Game::endPlayerTurn() {
    for (auto& champ : state.playerField) {
        if (champ.type == Card::CHAMPION) {
            if (champ.turnsInPlay == 0) {
                champ.turnsInPlay = 1;
            }
            champ.hasAttackedThisTurn = false;  // Reset attack flag
        }
    }

    // Check for Virtu-Machina synergy effects at turn end
    int vmCount = 0;
    for (const auto& card : state.playerField) {
        if (card.type == Card::CHAMPION && card.faction == Card::Faction::VIRTU_MACHINA) {
            vmCount++;
        }
    }
    
    if (vmCount >= 2) {  // If VM synergy is active
        int synergyLevel = calculateSynergyLevel(state.playerField, Card::Faction::VIRTU_MACHINA);
        state.playerEnergy += synergyLevel;  // Energy buff
        increaseTensorGauge(1);  // Tensor increase from VM synergy
    }

    state.isPlayerTurn = false;
    state.playerEnergy += 1;
    drawCard(state.playerHand);
    checkAndApplySynergies(state.playerField, state.playerHand);  // Recheck synergies

    mvprintw(LINES-1, 2, "Ending your turn...");
    refresh();
    napms(1500);
    
    // Check for tensor peak before enemy turn
    if (state.tensor.current >= state.tensor.maximum) {
        handleTensorPeak();
    }
    
    performEnemyTurn();
}

// Remove these duplicate function implementations since they exist in their respective files
/* void Game::performEnemyTurn() {
    // Remove this stub implementation - real one is in ai.cpp
}

void Game::playCardFromHand(int cardIndex) {
    // Remove this stub implementation - real one is in card.cpp
} */