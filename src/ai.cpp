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

    // Smarter card playing: Try to maintain synergies
    std::map<Card::Faction, int> factionCounts;
    for (const auto& card : state.enemyField) {
        if (card.type == Card::CHAMPION) {
            factionCounts[card.faction]++;
        }
    }

    // First try to play cards that would complete synergies
    for (size_t i = 0; i < state.enemyHand.size(); i++) {
        const auto& card = state.enemyHand[i];
        if (card.type == Card::CHAMPION && 
            factionCounts[card.faction] >= 1 && 
            card.cost <= state.enemyEnergy) {
            mvprintw(LINES-1, 2, "Enemy is playing a card...");
            refresh();
            napms(1000);
            playEnemyCard(i);
            playedCard = true;
            break;
        }
    }

    // If no synergy plays, then play highest cost affordable card
    if (!playedCard) {
        int bestIdx = -1;
        int highestCost = -1;
        for (size_t i = 0; i < state.enemyHand.size(); i++) {
            const auto& card = state.enemyHand[i];
            if (card.cost <= state.enemyEnergy && card.cost > highestCost) {
                highestCost = card.cost;
                bestIdx = i;
            }
        }
        if (bestIdx != -1) {
            mvprintw(LINES-1, 2, "Enemy is playing a card...");
            refresh();
            napms(1000);
            playEnemyCard(bestIdx);
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

    // Apply VM synergy effects at turn end
    int vmCount = 0;
    for (const auto& card : state.enemyField) {
        if (card.type == Card::CHAMPION && card.faction == Card::Faction::VIRTU_MACHINA) {
            vmCount++;
        }
    }
    
    if (vmCount >= 2) {
        int synergyLevel = calculateSynergyLevel(state.enemyField, Card::Faction::VIRTU_MACHINA);
        state.enemyEnergy += synergyLevel;
        increaseTensorGauge(1);
    }

    // Apply synergies and check VM effects
    checkAndApplySynergies(state.enemyField, state.enemyHand);
    
    // Check for tensor peak at end of turn
    if (state.tensor.current >= state.tensor.maximum) {
        handleTensorPeak();
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
                // Apply EXEC faction cost reduction if applicable
                int originalCost = card.cost;
                if (card.faction == Card::Faction::EXEC) {
                    int execCount = 0;
                    for (const auto& fieldCard : state.enemyField) {
                        if (fieldCard.type == Card::CHAMPION && 
                            fieldCard.faction == Card::Faction::EXEC) {
                            execCount++;
                        }
                    }
                    if (execCount >= 2) {
                        card.cost = std::max(0, card.cost - (execCount - 1));
                    }
                }

                state.enemyEnergy -= card.cost;
                state.enemyField.push_back(card);
                state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
                card.turnsInPlay = 0;
                checkAndApplySynergies(state.enemyField, state.enemyHand);
                
                mvprintw(LINES-1, 2, "Enemy plays %s (ATK:%d HP:%d)", 
                    card.name.c_str(), card.attack, card.health);
                refresh();
                napms(1500);
            }
            break;
            
        case Card::TENSOR:
            state.enemyEnergy += card.effect;
            state.enemyHand.erase(state.enemyHand.begin() + cardIndex);
            increaseTensorGauge(1);  // Tensor shard increases gauge
            mvprintw(LINES-1, 2, "Enemy uses %s for %d energy", 
                card.name.c_str(), card.effect);
            refresh();
            napms(1500);
            break;
    }
}