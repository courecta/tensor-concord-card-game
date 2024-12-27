#include "game.h"

// Card static methods implementation
Card Card::createChampion(const std::string& name, int cost, int attack, int health)
{
    return Card{CHAMPION, name, cost, attack, health, 0};
}

Card Card::createTensor(const std::string& name, int cost, int energyAmount)
{
    return Card{TENSOR, name, cost, 0, 0, energyAmount};
}

Card Card::createArtifact(const std::string& name, int cost, int effectValue)
{
    return Card{ARTIFACT, name, cost, 0, 0, effectValue};
}

// Game class implementation
Game::Game() : rng(std::chrono::steady_clock::now().time_since_epoch().count())
{
    initializeUI();
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

int Game::getMenuChoice(const std::vector<std::string>& options, int startY) {
    int choice = 0;
    int maxChoice = options.size() - 1;

    while(true) {
        // Clear previous options
        for(int i = 0; i <= maxChoice; i++) {
            mvprintw(startY + i, 0, "%-40s", "");
        }
        
        // Print options
        for(size_t i = 0; i < options.size(); i++) {
            mvprintw(startY + i, 2, "%c %s", 
                    (i == choice ? '>' : ' '), 
                    options[i].c_str());
        }
        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                choice = (choice > 0) ? choice - 1 : maxChoice;
                break;
            case KEY_DOWN:
                choice = (choice < maxChoice) ? choice + 1 : 0;
                break;
            case '\n':
                return choice;
            case 27:  // ESC
                return -1;
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

    state.isPlayerTurn = false;
    state.playerEnergy += 1;
    drawCard(state.playerHand);

    mvprintw(LINES-1, 2, "Ending your turn...");
    refresh();
    napms(1500);
    performEnemyTurn();
}