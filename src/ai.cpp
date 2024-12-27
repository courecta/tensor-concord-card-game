#include "game.h"

void Game::performEnemyTurn() {
    mvprintw(LINES-1, 2, "Enemy turn...");
    refresh();
    napms(1000);  // 1 second delay

    // Reset enemy champion states
    for (auto& champ : state.enemyField) {
        if (champ.type == Card::CHAMPION) {
            if (champ.turnsInPlay == 0) {
                champ.turnsInPlay = 1;
            }
            champ.hasAttackedThisTurn = false;
        }
    }

    // Try to play cards first
    bool playedCard = false;
    for (size_t i = 0; i < state.enemyHand.size(); i++) {
        if (state.enemyHand[i].cost <= state.enemyEnergy) {
            mvprintw(LINES-1, 2, "Enemy is playing a card...");
            refresh();
            napms(1000);
            playEnemyCard(i);
            playedCard = true;
            break;
        }
    }

    // Then try to attack with each champion
    for (auto& champ : state.enemyField) {
        if (champ.type == Card::CHAMPION && !champ.hasAttackedThisTurn && champ.turnsInPlay > 0) {
            // Simple AI: Attack player directly if no defenders, otherwise attack first defender
            if (state.playerField.empty()) {
                state.playerHealth -= champ.attack;
                mvprintw(LINES-1, 2, "Enemy %s attacks you directly for %d damage!", 
                        champ.name.c_str(), champ.attack);
            } else {
                Card& target = state.playerField[0];
                target.health -= champ.attack;
                mvprintw(LINES-1, 2, "Enemy %s attacks your %s for %d damage!", 
                        champ.name.c_str(), target.name.c_str(), champ.attack);
                
                if (target.health <= 0) {
                    state.playerField.erase(state.playerField.begin());
                    mvprintw(LINES-1, 2, "Your %s was destroyed!", target.name.c_str());
                }
            }
            champ.hasAttackedThisTurn = true;
            refresh();
            napms(1500);
        }
    }

    // End turn
    state.isPlayerTurn = true;
    state.enemyEnergy += 1;
    drawCard(state.enemyHand);
    
    mvprintw(LINES-1, 2, "Enemy turn ended.");
    refresh();
    napms(1000);
    state.printGameState();
}

void Game::playEnemyCard(int cardIndex) {
    Card& card = state.enemyHand[cardIndex];
    
    switch (card.type) {
        case Card::CHAMPION:
            if (state.enemyField.size() < MAX_FIELD_SIZE) {
                state.enemyEnergy -= card.cost;
                state.enemyField.push_back(card);
                state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
                card.turnsInPlay = 0;
                mvprintw(LINES-1, 2, "Enemy plays %s (ATK:%d HP:%d)", 
                    card.name.c_str(), card.attack, card.health);
                refresh();
                napms(1500);
            }
            break;
            
        case Card::TENSOR:
            state.enemyEnergy += card.effect;
            state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
            mvprintw(LINES-1, 2, "Enemy uses %s for %d energy", 
                card.name.c_str(), card.effect);
            refresh();
            napms(1500);
            break;
    }
}