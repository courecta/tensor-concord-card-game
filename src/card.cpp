#include "game.h"

void Game::drawCard(std::vector<Card>& hand) {
    if (state.deck.empty()) {
        clear();
        mvprintw(LINES/2, (COLS-30)/2, "Deck is empty!");
        if (state.playerHealth > state.enemyHealth) {
            mvprintw(LINES/2+1, (COLS-30)/2, "You have more HP - You win!");
            state.enemyHealth = 0;
        } else if (state.enemyHealth > state.playerHealth) {
            mvprintw(LINES/2+1, (COLS-30)/2, "Enemy has more HP - You lose!");
            state.playerHealth = 0;
        } else {
            mvprintw(LINES/2+1, (COLS-30)/2, "It's a tie!");
            state.playerHealth = state.enemyHealth = 0;
        }
        refresh();
        napms(2000);
        return;
    }
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
        mvprintw(LINES-1, 2, "Not enough energy! Card costs %d, you have %d", 
            card.cost, state.playerEnergy);
        refresh();
        napms(1500);
        return;
    }

    switch (card.type) {
        case Card::CHAMPION:
            if (state.playerField.size() < MAX_FIELD_SIZE) {
                state.playerEnergy -= card.cost;
                state.playerField.push_back(card);
                state.playerHand.erase(state.playerHand.begin() + cardIndex);
                card.turnsInPlay = 0;  // newly placed champion
                mvprintw(LINES-1, 2, "Played %s to field", card.name.c_str());
            } else {
                mvprintw(LINES-1, 2, "Field is full!");
            }
            break;

        case Card::ARTIFACT: {
            if (state.playerField.empty()) {
                mvprintw(LINES-1, 2, "No champions on field to buff!");
                refresh();
                napms(1500);
                return;
            }

            int selected = 0;
            while(true) {
                state.printGameState();
                
                // Show current stats and potential buff
                for(size_t i = 0; i < state.playerField.size(); i++) {
                    const auto& target = state.playerField[i];
                    GameUI::drawCard(stdscr, LINES/2+2, 2 + (i * 10), target, i == selected);
                    if (i == selected) {
                        if (card.effect % 2 == 1) {
                            mvprintw(LINES-3, 2, "Will buff ATK from %d to %d", 
                                    target.attack, target.attack + card.effect);
                        } else {
                            mvprintw(LINES-3, 2, "Will buff HP from %d to %d", 
                                    target.health, target.health + card.effect);
                        }
                    }
                }

                GameUI::drawStatusBar("Select target (Left/Right to choose, Enter to confirm, ESC to cancel)");
                refresh();

                int ch = getch();
                switch(ch) {
                    case KEY_LEFT:
                        selected = (selected > 0) ? selected - 1 : state.playerField.size() - 1;
                        break;
                    case KEY_RIGHT:
                        selected = (selected < state.playerField.size() - 1) ? selected + 1 : 0;
                        break;
                    case '\n': {
                        state.playerEnergy -= card.cost;
                        if (card.effect % 2 == 1) {
                            state.playerField[selected].attack += card.effect;
                        } else {
                            state.playerField[selected].health += card.effect;
                        }
                        state.playerHand.erase(state.playerHand.begin() + cardIndex);
                        mvprintw(LINES-1, 2, "Buffed %s", state.playerField[selected].name.c_str());
                        refresh();
                        napms(1000);
                        return;
                    }
                    case 27:
                        return;
                }
            }
            break;
        }

        case Card::TENSOR:
            int oldEnergy = state.playerEnergy;
            state.playerEnergy += card.effect;
            increaseTensorGauge(1);  // Add this line to properly increase tensor
            state.playerHand.erase(state.playerHand.begin() + cardIndex);
            mvprintw(LINES-1, 2, "Used Tensor Shard: Energy %d → %d", 
                    oldEnergy, state.playerEnergy);
            break;
    }
    refresh();
    napms(1000);
    state.printGameState();
}

void Game::attackWithCard(int cardIndex) {
    if (cardIndex < 0 || cardIndex >= static_cast<int>(state.playerField.size())) {
        std::cout << "Invalid card index\n";
        return;
    }

    Card& attacker = state.playerField[cardIndex];
    if(attacker.hasAttackedThisTurn) {
        mvprintw(LINES-1, 2, "This champion has already attacked this turn!");
        refresh();
        napms(1500);
        return;
    }

    int selected = 0;
    while(true) {
        state.printGameState();

        // Show attacker info in a better format
        attron(A_BOLD);
        mvprintw(LINES-4, 2, "Attacking with: %s", state.playerField[cardIndex].name.c_str());
        attroff(A_BOLD);
        mvprintw(LINES-4, 2 + 14 + state.playerField[cardIndex].name.length(), 
                " (ATK: %d)", state.playerField[cardIndex].attack);

        // Show targets in a cleaner horizontal layout
        mvprintw(LINES-3, 2, "Select target:");
        
        // Draw targets with better spacing and info
        int xPos = 2;
        if(selected == 0) attron(A_REVERSE);
        mvprintw(LINES-2, xPos, "[ Direct Attack ]");
        if(selected == 0) attroff(A_REVERSE);

        for(size_t i = 0; i < state.enemyField.size(); i++) {
            const auto& target = state.enemyField[i];
            xPos = 2 + ((i + 1) * 25);
            
            if(selected == i + 1) attron(A_REVERSE);
            mvprintw(LINES-2, xPos, "[ %s HP:%d ]", 
                    target.name.c_str(), target.health);
            if(selected == i + 1) attroff(A_REVERSE);
        }

        refresh();
        int ch = getch();
        switch(ch) {
            case KEY_LEFT:
                selected = (selected > 0) ? selected - 1 : state.enemyField.size();
                break;
            case KEY_RIGHT:
                selected = (selected < state.enemyField.size()) ? selected + 1 : 0;
                break;
            case '\n':
                if(state.playerField[cardIndex].turnsInPlay == 0) {
                    mvprintw(LINES-1, 2, "Champion can't attack on the turn it was played!");
                    refresh();
                    napms(1500);
                    return;
                }
                
                if (selected == 0) {
                    state.enemyHealth -= state.playerField[cardIndex].attack;
                    mvprintw(LINES-1, 2, "%s attacks enemy directly for %d damage!", 
                            attacker.name.c_str(), 
                            attacker.attack);
                } else {
                    int enemyIndex = selected - 1;
                    Card& defender = state.enemyField[enemyIndex];
                    defender.health -= attacker.attack;
                    
                    // Use consistent name formatting like in GameUI::drawCard
                    mvprintw(LINES-1, 2, "%s attacks %s for %d damage!", 
                            attacker.name.c_str(),
                            defender.name.c_str(),
                            attacker.attack);
                    
                    if(defender.health <= 0) {
                        napms(1000);
                        mvprintw(LINES-1, 2, "%s was destroyed!", defender.name.c_str());
                        state.enemyField.erase(state.enemyField.begin() + enemyIndex);
                    }
                }
                state.playerField[cardIndex].hasAttackedThisTurn = true;  // Modified this line
                refresh();
                napms(1500);
                return;
            case 27:
                return;
        }
    }
    state.printGameState();
}