#include "game.h"

void Game::printHelp() const {
    std::cout << "\nCommands:\n"
              << "  play <index>   - Play card from hand\n"
              << "  attack <myChampionIndex> <targetIndex> - Attack with card on field\n"
              << "  end           - End your turn\n"
              << "  help          - Show this help\n"
              << "  quit          - Exit game\n\n";
}

void Game::showHelpMenu() const {
    std::vector<std::string> helpOptions = {
        "Commands",
        "Card Types",
        "Factions",
        "Tensor System",
        "Exit Help"
    };
    
    int choice = 0;
    bool running = true;

    while(running) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(2, (COLS-9)/2, "Help Menu");
        GameUI::drawCenteredMenu(helpOptions, choice);
        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP: choice = (choice > 0) ? choice - 1 : 4; break;
            case KEY_DOWN: choice = (choice < 4) ? choice + 1 : 0; break;
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
                    mvprintw(1, 2, "Card Types & Roles:");
                    mvprintw(3, 2, "CHAMPION - Has attack, health, and cost");
                    mvprintw(4, 2, "  Roles: MERC, NOMAD, CORPO, MAGE");
                    mvprintw(5, 2, "ARTIFACT - Neural/Bio enhancements");
                    mvprintw(6, 2, "TENSOR   - Data shard energy boost");
                    mvprintw(LINES-1, 2, "Press any key to continue...");
                    refresh();
                    getch();
                }
                else if(choice == 2) {
                    clear();
                    mvprintw(1, 2, "Factions:");
                    mvprintw(3, 2, "TECHNO - ATK buff (+1/+2/+3)");
                    mvprintw(4, 2, "CYBER - HP buff (+1/+2/+3)");
                    mvprintw(5, 2, "EXEC - Cost reduction (-1/-2/-3)");
                    mvprintw(6, 2, "VIRTU-MACHINA - Energy & Tensor buffs");
                    mvprintw(8, 2, "2 same faction = Level 1");
                    mvprintw(9, 2, "3 same faction = Level 2");
                    mvprintw(10, 2, "4 same faction = Level 3");
                    mvprintw(LINES-1, 2, "Press any key to continue...");
                    refresh();
                    getch();
                }
                else if(choice == 3) {
                    clear();
                    mvprintw(1, 2, "Tensor System:");
                    mvprintw(3, 2, "- Peaks at maximum value");
                    mvprintw(4, 2, "- Triggers minigame at peak");
                    mvprintw(5, 2, "- Maximum increases after peak");
                    mvprintw(6, 2, "- Starts at 3, caps at 7");
                    mvprintw(8, 2, "Increased by:");
                    mvprintw(9, 2, "- Tensor Shards");
                    mvprintw(10, 2, "- VM Faction Synergy");
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

int GameUI::getFactionColor(Card::Faction faction) {
    switch(faction) {
        case Card::Faction::TECHNO: return COLOR_RED;
        case Card::Faction::CYBER: return COLOR_GREEN;
        case Card::Faction::EXEC: return COLOR_YELLOW;
        case Card::Faction::VIRTU_MACHINA: return COLOR_BLUE;
        default: return 0;
    }
}

void GameUI::drawCard(WINDOW* win, int y, int x, const Card& card, bool isSelected) {
    // Simplified borders using basic ASCII
    const char* border;
    if (card.type == Card::CHAMPION) {
        if (card.turnsInPlay == 0) {
            border = "........."; // New card
        } else if (card.hasAttackedThisTurn) {
            border = "........."; // Used card
        } else {
            border = isSelected ? "=========" : "---------"; // Ready card
        }
    } else {
        border = isSelected ? "=========" : "---------";
    }

    // Only color top border for synergy
    if (card.type == Card::CHAMPION && card.hasSynergyBuff) {
        attron(COLOR_PAIR(FACTION_COLOR_START + static_cast<int>(card.faction)));
        mvprintw(y, x, "+%s+", border);
        attroff(COLOR_PAIR(FACTION_COLOR_START + static_cast<int>(card.faction)));
    } else {
        mvprintw(y, x, "+%s+", border);
    }

    // Draw card contents in default color
    attroff(COLOR_PAIR(FACTION_COLOR_START + static_cast<int>(card.faction)));
    
    // Format full name properly
    std::string factionNames[] = {"None", "Techno", "Cyber", "Exec", "Virtu-Machina"};
    std::string roleNames[] = {"None", "Merc", "Nomad", "Corpo", "Mage"};
    std::string fullName = factionNames[static_cast<int>(card.faction)] + 
                          " - " + 
                          roleNames[static_cast<int>(card.role)];
    
    mvprintw(y + 1, x, "|%-22s|", fullName.c_str());
    
    if (card.type == Card::CHAMPION) {
        mvprintw(y + 2, x, "|ATK:");
        if (card.attack > card.originalAttack) {  // Compare with original stats
            attron(COLOR_PAIR(BUFF_COLOR));
            printw("%2d", card.attack);
            attroff(COLOR_PAIR(BUFF_COLOR));
        } else {
            printw("%2d", card.attack);
        }
        printw(" |");
        
        mvprintw(y + 3, x, "|HP:");
        if (card.health > card.originalHealth) {  // Compare with original stats
            attron(COLOR_PAIR(BUFF_COLOR));
            printw("%3d", card.health);
            attroff(COLOR_PAIR(BUFF_COLOR));
        } else {
            printw("%3d", card.health);
        }
        printw(" |");
    } else if (card.type == Card::ARTIFACT) {
        mvprintw(y + 2, x, "|BUFF+%d |", card.effect);
        mvprintw(y + 3, x, "|Cost:%2d|", card.cost);
    } else {  // TENSOR
        mvprintw(y + 2, x, "|NRG+%d |", card.effect);
        mvprintw(y + 3, x, "|Free   |");
    }
    mvprintw(y + 4, x, "+%s+", border);
}

void GameUI::drawField(WINDOW* win, int y, int x, const std::vector<Card>& field, int selectedIndex) {
    for(size_t i = 0; i < field.size(); ++i) {
        drawCard(win, y, x + (i * 20), field[i], i == selectedIndex);  // Reduced spacing to 20
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
        int xPos = x + ((i - startIndex) * 20);  // Use consistent 20-space width
        
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        if(isSelected) attron(A_REVERSE | A_BOLD);
        
        // Format name consistently with field display
        std::string factionNames[] = {"None", "Techno", "Cyber", "Exec", "Virtu-Machina"};
        std::string roleNames[] = {"None", "Merc", "Nomad", "Corpo", "Mage"};
        std::string fullName;
        
        if (card.type == Card::CHAMPION) {
            fullName = factionNames[static_cast<int>(card.faction)] + 
                      " - " + 
                      roleNames[static_cast<int>(card.role)];
        } else {
            fullName = card.name;  // Keep original name for non-champions
        }
        
        switch(card.type) {
            case Card::CHAMPION:
                mvprintw(y, xPos, "%-20s", fullName.c_str());
                mvprintw(y + 1, xPos, "ATK: %-3d  HP: %-3d", card.attack, card.health);
                mvprintw(y + 2, xPos, "Cost: %d", card.cost);
                break;
            case Card::ARTIFACT:
                mvprintw(y, xPos, "%s", card.name.c_str());
                mvprintw(y + 1, xPos, "Buff: +%d", card.effect);
                mvprintw(y + 2, xPos, "Cost: %d", card.cost);  // Fixed: Add missing cost parameter
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

void GameUI::initializeAllColors() {
    start_color();
    use_default_colors();
    
    // Initialize tensor rainbow colors (1-7)
    for (int i = 0; i < 7; i++) {
        init_pair(TENSOR_COLOR_START + i, GameState::TensorState::RAINBOW_COLORS[i], COLOR_BLACK);
    }
    
    // Initialize faction colors (8-11)
    init_pair(FACTION_COLOR_START + static_cast<int>(Card::Faction::TECHNO), COLOR_RED, COLOR_BLACK);
    init_pair(FACTION_COLOR_START + static_cast<int>(Card::Faction::CYBER), COLOR_GREEN, COLOR_BLACK);
    init_pair(FACTION_COLOR_START + static_cast<int>(Card::Faction::EXEC), COLOR_YELLOW, COLOR_BLACK);
    init_pair(FACTION_COLOR_START + static_cast<int>(Card::Faction::VIRTU_MACHINA), COLOR_BLUE, COLOR_BLACK);
    
    // Initialize buff color (12)
    init_pair(BUFF_COLOR, COLOR_GREEN, COLOR_BLACK);
}

void GameUI::drawTensorGauge(int y, int x, int current, int maximum) {
    mvprintw(y, x, "Tensor: [");
    
    // Draw filled portion with rainbow colors
    for(int i = 0; i < current; i++) {
        attron(COLOR_PAIR(i % 7 + 1));
        addch(' ' | A_REVERSE);
        attroff(COLOR_PAIR(i % 7 + 1));
    }
    
    // Draw empty portion
    for(int i = current; i < maximum; i++) {
        addch('-');
    }
    
    printw("]");
}

void GameUI::drawCenteredMenu(const std::vector<std::string>& options, int selected) {
    int width = 30;  // Fixed width for menu boxes
    int startY = (LINES - options.size()) / 2;  // Center vertically
    int startX = (COLS - width) / 2;  // Center horizontally

    for (size_t i = 0; i < options.size(); i++) {
        if (i == selected) {
            attron(A_REVERSE);
            mvprintw(startY + i, startX, "[%*s]", width-2, options[i].c_str());
            attroff(A_REVERSE);
        } else {
            mvprintw(startY + i, startX, "[ %-*s ]", width-4, options[i].c_str());
        }
    }
    refresh();
}