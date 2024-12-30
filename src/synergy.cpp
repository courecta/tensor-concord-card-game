#include "game.h"

void Game::checkAndApplySynergies(std::vector<Card>& field, std::vector<Card>& hand) {
    std::map<Card::Faction, int> factionCount;
    std::set<Card::Faction> uniqueFactions;
    
    // Count factions and track unique ones
    for (const auto& card : field) {
        if (card.type == Card::CHAMPION && card.faction != Card::Faction::FACTION_NONE) {
            factionCount[card.faction]++;
            uniqueFactions.insert(card.faction);
        }
    }
    
    // Check Tensor Concordia first (all factions present)
    bool tensorConcordiaActive = uniqueFactions.size() == 4;  // All 4 factions
    
    // Apply regular synergies
    for (const auto& pair : factionCount) {
        if (pair.second >= 2) {
            int synergyLevel = calculateSynergyLevel(field, pair.first);
            applySynergyEffects(field, hand, pair.first, synergyLevel);
        }
    }

    // Apply Tensor Concordia bonus if active
    if (tensorConcordiaActive) {
        // Add dramatic visual effect
        attron(A_BOLD | A_BLINK);
        mvprintw(LINES/2, (COLS-40)/2, "* TENSOR CONCORDIA ACTIVATED *");
        attroff(A_BOLD | A_BLINK);
        refresh();
        napms(1500);
        
        for (auto& card : field) {
            if (card.type == Card::CHAMPION) {
                card.attack += 3;
                card.health += 3;
                card.hasSynergyBuff = true;
            }
        }
        // Log the activation
        mvprintw(LINES-1, 2, "Tensor Concordia active! All champions +3/+3");
        refresh();
        napms(1500);
    }
}

int Game::calculateSynergyLevel(const std::vector<Card>& field, Card::Faction faction) {
    int count = 0;
    for (const auto& card : field) {
        if (card.type == Card::CHAMPION && card.faction == faction) {
            count++;
        }
    }
    
    // 2 cards = level 1, 3 cards = level 2, 4 cards = level 3
    return std::min(count - 1, 3);
}

void Game::applySynergyEffects(std::vector<Card>& field, std::vector<Card>& hand, Card::Faction faction, int level) {
    // First check for Tensor Concordia (all factions present)
    std::set<Card::Faction> uniqueFactions;
    for (const auto& card : field) {
        if (card.type == Card::CHAMPION && card.faction != Card::Faction::FACTION_NONE) {
            uniqueFactions.insert(card.faction);
        }
    }

    bool tensorConcordiaActive = uniqueFactions.size() == 4;  // All 4 factions present

    // Reset stats and apply new buffs
    for (auto& card : field) {
        if (card.type == Card::CHAMPION) {
            card.attack = card.originalAttack;
            card.health = card.originalHealth;
            
            if (card.faction == faction) {
                switch (faction) {
                    case Card::Faction::TECHNO:
                        card.attack += level;
                        break;
                    case Card::Faction::CYBER:
                        card.health += level;
                        break;
                    case Card::Faction::VIRTU_MACHINA:
                        // VM specific effects handled in endTurn
                        break;
                }
            }

            // Apply Tensor Concordia buff if active
            if (tensorConcordiaActive) {
                card.attack += 3;
                card.health += 3;
                card.hasSynergyBuff = true;
            }
        }
    }

    // Handle EXEC cost reduction
    if (faction == Card::Faction::EXEC) {
        for (auto& card : hand) {
            if (card.faction == Card::Faction::EXEC) {
                card.cost = std::max(0, card.cost - level);
            }
        }
    }
}
