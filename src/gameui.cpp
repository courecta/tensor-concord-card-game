#include "game.h"
#include "gameui.h"

void Game::printHelp() const {
    std::cout << "\nCommands:\n"
              << "  play <index>   - Play card from hand\n"
              << "  attack <myChampionIndex> <targetIndex> - Attack with card on field\n"
              << "  end           - End your turn\n"
              << "  help          - Show this help\n"
              << "  quit          - Exit game\n\n";
}

void Game::showHelpMenu() const {
    const char* helpOptions[] = {
        "Commands",
        "Card Types",
        "Exit Help"
    };
    int choice = 0;
    bool running = true;

    while(running) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(1, (COLS-9)/2, "Help Menu");
        mvhline(2, 1, ACS_HLINE, COLS-2);

        for(int i = 0; i < 3; i++) {
            mvprintw(i+4, 2, "%c %s", (i == choice ? '>' : ' '), helpOptions[i]);
        }
        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP: choice = (choice > 0) ? choice - 1 : 2; break;
            case KEY_DOWN: choice = (choice < 2) ? choice + 1 : 0; break;
            case '\n':
                if(choice == 0) {
                    clear();
                    mvprintw(1, 2, "Commands:");
                    mvprintw(3, 2, "[Play card]");
                    mvprintw(4, 2, "[Attack]");
                    mvprintw(5, 2, "[End turn]");
                    mvprintw(6, 2, "[Help]");
                    mvprintw(7, 2, "[Quit]");
                    mvprintw(LINES-1, 2, "Press any key to continue...");
                    refresh();
                    getch();
                } 
                else if(choice == 1) {
                    clear();
                    mvprintw(1, 2, "Card Types:");
                    mvprintw(3, 2, "CHAMPION - Has attack, health, and cost");
                    mvprintw(4, 2, "ARTIFACT - Buffs a champion on the field");
                    mvprintw(5, 2, "TENSOR   - Provides immediate energy boost");
                    mvprintw(LINES-1, 2, "Press any key to continue...");
                    refresh();
                    getch();
                }
                else {
                    running = false;
                }
                break;
            case 27: // ESC
                running = false;
                break;
        }
    }
    state.printGameState();
}

void GameUI::drawBox(int y, int x, int height, int width) {
    for(int i = y; i < y + height; ++i) {
        mvhline(i, x, 0, width);
    }
    box(stdscr, 0, 0);
}

void GameUI::drawCard(WINDOW* win, int y, int x, const Card& card, bool isSelected) {
    const char* border;
    if (card.type == Card::CHAMPION) {
        if (card.hasAttackedThisTurn) {
            border = "..........";  // Used champion
        } else if (card.turnsInPlay == 0) {
            border = "..........";  // Can't attack yet
        } else {
            border = isSelected ? "========" : "--------";
        }
    } else {
        border = isSelected ? "========" : "--------";
    }
    
    mvprintw(y, x, "%s", border);
    mvprintw(y + 1, x, "|%8s|", card.name.substr(0, 8).c_str());
    
    if (card.type == Card::CHAMPION) {
        mvprintw(y + 2, x, "|ATK:%2d |", card.attack);
        mvprintw(y + 3, x, "|HP:%3d |", card.health);
    } else if (card.type == Card::ARTIFACT) {
        mvprintw(y + 2, x, "|BUFF+%d |", card.effect);
        mvprintw(y + 3, x, "|Cost:%2d|", card.cost);
    } else {  // TENSOR
        mvprintw(y + 2, x, "|NRG+%d |", card.effect);
        mvprintw(y + 3, x, "|Free   |");
    }
    mvprintw(y + 4, x, "%s", border);
}

void GameUI::drawField(WINDOW* win, int y, int x, const std::vector<Card>& field, int selectedIndex) {
    for(size_t i = 0; i < field.size(); ++i) {
        drawCard(win, y, x + (i * 10), field[i], i == selectedIndex);
    }
}

void GameUI::drawStats(WINDOW* win, int y, int x, int health, int energy) {
    mvprintw(y, x, "Health: %d | Energy: %d", health, energy);
}

void GameUI::drawActionMenu(int selectedAction) {
    const char* actions[] = {"[Play]", "[Attack]", "[End Turn]", "[Help]", "[Quit]"};
    int menuY = LINES - 2;
    int spacing = COLS / (sizeof(actions) / sizeof(actions[0]));
    
    for(int i = 0; i < 5; i++) {
        int x = (spacing * i) + (spacing/2) - strlen(actions[i])/2;
        if(i == selectedAction) {
            attron(A_REVERSE);
            mvprintw(menuY, x, "%s", actions[i]);
            attroff(A_REVERSE);
        } else {
            mvprintw(menuY, x, "%s", actions[i]);
        }
    }
}

void GameUI::drawStatusBar(const std::string& status) {
    mvprintw(LINES-1, 2, "%-*s", COLS-52, status.c_str());  // Adjusted spacing
}

void GameUI::drawNavigationHints() {
    int navX = COLS - 48;  // More space for controls
    mvaddch(LINES-1, navX - 2, ACS_VLINE);
    attron(A_DIM);
    mvprintw(LINES-1, navX, "Controls: <--/--> | ENTER Confirm - ESC Cancel");
    attroff(A_DIM);
}

void GameUI::drawHand(WINDOW* win, int y, int x, const std::vector<Card>& hand, int selectedIndex) {
    const size_t cardsPerPage = 4;
    size_t currentPage = selectedIndex >= 0 ? selectedIndex / cardsPerPage : 0;
    size_t startIndex = currentPage * cardsPerPage;
    size_t endIndex = std::min(startIndex + cardsPerPage, hand.size());
    
    // Clear the hand area first
    for(int i = 0; i < 4; i++) {
        mvhline(y + i, x, ' ', COLS - x - 32);  // Leave space for info panel
    }
    
    // Draw cards with better formatting
    for(size_t i = startIndex; i < endIndex; i++) {
        const auto& card = hand[i];
        int xPos = x + ((i - startIndex) * 25);
        
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        if(isSelected) attron(A_REVERSE | A_BOLD);
        
        switch(card.type) {
            case Card::CHAMPION:
                mvprintw(y, xPos, "%s", card.name.c_str());
                mvprintw(y + 1, xPos, "ATK: %-3d  HP: %-3d", card.attack, card.health);
                mvprintw(y + 2, xPos, "Cost: %d", card.cost);
                break;
            case Card::ARTIFACT:
                mvprintw(y, xPos, "%s", card.name.c_str());
                mvprintw(y + 1, xPos, "Buff: +%d", card.effect);
                mvprintw(y + 2, xPos, "Cost: %d", card.cost);
                break;
            case Card::TENSOR:
                mvprintw(y, xPos, "%s", card.name.c_str());
                mvprintw(y + 1, xPos, "Energy: +%d", card.effect);
                mvprintw(y + 2, xPos, "Cost: FREE");
                break;
        }
        
        if(isSelected) attroff(A_REVERSE | A_BOLD);
    }
    
    // Only show page navigation when needed and in selection mode
    if(selectedIndex >= 0 && hand.size() > cardsPerPage) {
        attron(A_DIM);
        if(startIndex > 0) {
            mvprintw(y + 3, x, "<< More");
        }
        if(endIndex < hand.size()) {
            mvprintw(y + 3, x + (cardsPerPage * 25) - 8, "More >>");
        }
        attroff(A_DIM);
    }
}

void GameUI::drawHealthBar(int y, int x, int current, int max, bool isEnergy) {
    const char* symbol = isEnergy ? "+" : "#";  // Using simpler symbols for compatibility
    const char* label = isEnergy ? "Energy" : "Health";
    
    mvprintw(y, x, "%s: [", label);
    attron(A_BOLD);
    for(int i = 0; i < max; i++) {
        if(i < current) {
            attron(isEnergy ? A_BOLD : A_REVERSE);
            addch(symbol[0]);
            attroff(isEnergy ? A_BOLD : A_REVERSE);
        } else {
            addch('-');
        }
    }
    attroff(A_BOLD);
    printw("] %d/%d", current, max);
}

void GameUI::highlightSelectedCard(int y, int x, int width, int height) {
    attron(A_REVERSE);
    for(int i = 0; i < height; i++) {
        mvhline(y + i, x, ' ', width);
    }
    attroff(A_REVERSE);
}