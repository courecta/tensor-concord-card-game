#include "minigames.h"

namespace {
    // Constants for dice positioning
    const int diceY = LINES/2;
    const int dice1X = (COLS-30)/2;
    const int dice2X = (COLS-10)/2;

    // Card values array for high-low game
    const char* ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
}

bool Game::playCoinToss() {
    clear();
    box(stdscr, 0, 0);
    
    mvprintw(2, (COLS-16)/2, "=== COIN TOSS ===");
    mvprintw(4, (COLS-20)/2, "Stakes: 2 Energy");
    
    const std::vector<std::string> choices = {"Heads", "Tails"};
    mvprintw(6, (COLS-25)/2, "Call it in the air!");
    
    int choice = showMenu(choices, "COIN TOSS");
    if(choice == -1) return false;
    
    bool isHeads = (rand() % 2) == 0;
    bool won = (choice == 0 && isHeads) || (choice == 1 && !isHeads);
    
    // Animate coin toss
    const char* coinFrames[] = {
        "   _____   ",
        "  /     \\  ",
        " |       | ",
        "  \\     /  ",
        "   -----   "
    };
    
    for(int i = 0; i < 6; i++) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(2, (COLS-16)/2, "=== COIN TOSS ===");
        
        int y = LINES/2 - 2;
        for(const auto& line : coinFrames) {
            mvprintw(y++, (COLS-10)/2, "%s", line);
        }
        refresh();
        napms(200);
    }
    
    // Show coin result
    mvprintw(12, (COLS-20)/2, "The coin shows:");
    const char* coinArt = isHeads ? 
        "   _____ \n"
        "  /     \\\n"
        " | HEADS |\n"
        "  \\     /\n"
        "   -----  " :
        "   _____ \n"
        "  /     \\\n"
        " | TAILS |\n"
        "  \\     /\n"
        "   -----  ";
    
    int artY = 14;
    std::istringstream iss(coinArt);
    std::string line;
    while(std::getline(iss, line)) {
        mvprintw(artY++, (COLS-9)/2, "%s", line.c_str());
    }
    
    refresh();
    napms(1500);
    return won;
}

bool Game::playHighLow() {
    clear();
    box(stdscr, 0, 0);
    
    // Initialize colors and suits first
    init_pair(15, COLOR_RED, COLOR_BLACK);
    const char* suitNames[] = {"Spades", "Hearts", "Diamonds", "Clubs"};
// Constants for positioning
    const int TITLE_Y = 2;
    const int STAKES_Y = 4;
    const int FIRST_CARD_LABEL_Y = 8;
    const int FIRST_CARD_Y = 10;
    const int MENU_Y = LINES - 8;
    
    clear();
    box(stdscr, 0, 0);

    // Generate cards
    int firstSuit = rand() % 4;
    int firstRank = rand() % 13;
    int secondSuit = rand() % 4;
    int secondRank = rand() % 13;
    
    // Display title and first card
    mvprintw(TITLE_Y, (COLS-20)/2, "=== HIGH OR LOW ===");
    // mvprintw(STAKES_Y, (COLS-20)/2, "Stakes: 1 Health");
    
    std::string firstCard = std::string(ranks[firstRank]) + " of " + std::string(suitNames[firstSuit]);
    mvprintw(FIRST_CARD_LABEL_Y, (COLS-20)/2, "Current card:");
    
    if (firstSuit == 1 || firstSuit == 2) {
        attron(COLOR_PAIR(15));
        mvprintw(FIRST_CARD_Y, (COLS-firstCard.length())/2, "%s", firstCard.c_str());
        attroff(COLOR_PAIR(15));
    } else {
        mvprintw(FIRST_CARD_Y, (COLS-firstCard.length())/2, "%s", firstCard.c_str());
    }
    
    refresh();
    napms(5000); // Delay to ensure first card is visible
    
    // Show menu
    mvprintw(MENU_Y, (COLS-40)/2, "Will the next card be Higher or Lower?");
    std::vector<std::string> choices = {"Higher", "Lower"};
    int choice = showMenu(choices, "Card Guessing");
    if(choice == -1) return false;

    // Clear screen and redisplay title and first card
    clear();
    box(stdscr, 0, 0);
    mvprintw(2, (COLS-20)/2, "=== HIGH OR LOW ===");
    
    // Show both cards in upper portion
    mvprintw(4, (COLS-20)/2, "First card:");
    if (firstSuit == 1 || firstSuit == 2) {
        attron(COLOR_PAIR(15));
        mvprintw(5, (COLS-firstCard.length())/2, "%s", firstCard.c_str());
        attroff(COLOR_PAIR(15));
    } else {
        mvprintw(5, (COLS-firstCard.length())/2, "%s", firstCard.c_str());
    }

    mvprintw(7, (COLS-20)/2, "Second card:");
    std::string secondCard = std::string(ranks[secondRank]) + " of " + std::string(suitNames[secondSuit]);
    if (secondSuit == 1 || secondSuit == 2) {
        attron(COLOR_PAIR(15));
        mvprintw(8, (COLS-secondCard.length())/2, "%s", secondCard.c_str());
        attroff(COLOR_PAIR(15));
    } else {
        mvprintw(8, (COLS-secondCard.length())/2, "%s", secondCard.c_str());
    }

    // Compare cards logic remains the same
    bool isHigher;
    if (firstRank != secondRank) {
        if (firstRank == 12) {
            isHigher = false;
        } else if (secondRank == 12) {
            isHigher = true;
        } else {
            isHigher = secondRank > firstRank;
        }
    } else {
        isHigher = secondSuit < firstSuit;
    }

    bool won = (choice == 0 && isHigher) || (choice == 1 && !isHigher);

    // Display result in upper portion
    mvprintw(10, (COLS-40)/2, "The second card was %s!", isHigher ? "higher" : "lower");
    refresh();
    napms(1500);
    return won;
}

bool Game::playRoulette() {
    clear();
    box(stdscr, 0, 0);
    
    mvprintw(2, (COLS-16)/2, "=== ROULETTE ===");
    mvprintw(4, (COLS-20)/2, "Stakes: 3 Energy");
    
    std::vector<std::string> choices = {"1", "2", "3", "4", "5", "6"};
    mvprintw(LINES/2-2, (COLS-25)/2, "Choose your number:");
    
    int choice = showMenu(choices, "ROULETTE");
    if(choice == -1) return false;
    
    int result = (rand() % 6) + 1;
    
    // Simple number scroll animation
    mvprintw(LINES/2+1, (COLS-20)/2, "Rolling...");
    for(int i = 0; i < 12; i++) {
        mvprintw(LINES/2+2, (COLS-5)/2, "[%d]", (i % 6) + 1);
        refresh();
        napms(100 + (i * 20));  // Gradually slow down
    }
    
    mvprintw(LINES/2+2, (COLS-5)/2, "[%d]", result);
    mvprintw(LINES/2+4, (COLS-20)/2, "Final number: %d", result);
    refresh();
    napms(1500);
    return choice + 1 == result;
}

bool Game::playDiceRoll() {
    clear();
    box(stdscr, 0, 0);
    
    mvprintw(2, (COLS-16)/2, "=== DICE ROLL ===");
    mvprintw(4, (COLS-20)/2, "Stakes: 2 Health");
    
    std::vector<std::string> choices = {"High (8-12)", "Low (2-6)"};
    int choice = showMenu(choices, "DICE ROLL");
    if(choice == -1) return false;
    
    int dice1 = (rand() % 6) + 1;
    int dice2 = (rand() % 6) + 1;
    
    // Simple dice display
    const char* diceTemplate[] = {
        "+-----+",
        "|%s|",  // Middle will contain appropriate dice pattern
        "+-----+"
    };

    // Replace the dice patterns array with simpler reusable patterns:
    const char* dicePatterns[] = {
        // Basic patterns that will be reused
        "  O  ",  // Pattern 1 - center dot
        "O   O",  // Pattern 2 - diagonal dots
        "O O O"   // Pattern 3 - three dots
    };

    // Draw dice side by side
    int diceY = LINES/2 - 2;  // Move up slightly
    int dice1X = (COLS/2) - 15;
    int dice2X = (COLS/2) + 5;

    // Helper function to draw a single die
    auto drawDie = [&](int x, int value) {
        mvprintw(diceY, x, "+-----+");
        
        switch(value) {
            case 1:
                mvprintw(diceY+1, x, "|     |");
                mvprintw(diceY+2, x, "|  O  |");
                mvprintw(diceY+3, x, "|     |");
                break;
            case 2:
                mvprintw(diceY+1, x, "|  O  |");
                mvprintw(diceY+2, x, "|     |");
                mvprintw(diceY+3, x, "|  O  |");
                break;
            case 3:
                mvprintw(diceY+1, x, "|  O  |");
                mvprintw(diceY+2, x, "|  O  |");
                mvprintw(diceY+3, x, "|  O  |");
                break;
            case 4:  // Two 2's
                mvprintw(diceY+1, x, "|O   O|");
                mvprintw(diceY+2, x, "|     |");
                mvprintw(diceY+3, x, "|O   O|");
                break;
            case 5:  // 2 + 1 + 2
                mvprintw(diceY+1, x, "|O   O|");
                mvprintw(diceY+2, x, "|  O  |");
                mvprintw(diceY+3, x, "|O   O|");
                break;
            case 6:  // Two 3's
                mvprintw(diceY+1, x, "|O   O|");
                mvprintw(diceY+2, x, "|O   O|");
                mvprintw(diceY+3, x, "|O   O|");
                break;
        }
        mvprintw(diceY+4, x, "+-----+");
    };

    // Draw both dice
    drawDie(dice1X, dice1);
    drawDie(dice2X, dice2);

    int sum = dice1 + dice2;
    mvprintw(diceY+6, (COLS-40)/2, "Sum: %d", sum);
    mvprintw(diceY+7, (COLS-35)/2, "Press any key to continue...");
    
    bool isHigh = sum >= 8;
    bool won = (choice == 0 && isHigh) || (choice == 1 && !isHigh);
    
    refresh();
    napms(1500);
    return won;
}

// ...existing code...

bool Game::playRPS() {
    clear();
    box(stdscr, 0, 0);
    
    mvprintw(2, (COLS-24)/2, "=== ROCK PAPER SCISSORS ===");
    mvprintw(4, (COLS-20)/2, "Stakes: 1 Energy");
    
    const std::vector<std::string> choices = {"Rock", "Paper", "Scissors"};
    mvprintw(LINES/2-2, (COLS-25)/2, "Make your choice!");
    
    int choice = showMenu(choices, "ROCK PAPER SCISSORS");
    if(choice == -1) return false;
    
    int enemyChoice = rand() % 3;
    
    // Show choices clearly
    mvprintw(LINES/2+2, (COLS-30)/2, "You chose: %s", choices[choice].c_str());
    mvprintw(LINES/2+3, (COLS-30)/2, "Enemy chose: %s", choices[enemyChoice].c_str());
    
    if(choice == enemyChoice) {
        mvprintw(LINES/2+5, (COLS-20)/2, "It's a tie!");
        refresh();
        napms(1500);
        return playRPS();  // Play again on tie
    }
    
    bool won = (choice == 0 && enemyChoice == 2) ||  // Rock beats Scissors
               (choice == 1 && enemyChoice == 0) ||  // Paper beats Rock
               (choice == 2 && enemyChoice == 1);    // Scissors beats Paper
    
    refresh();
    napms(1500);
    return won;
}

void Game::applyMinigameConsequence(bool won) {
    clear();
    box(stdscr, 0, 0);
    
    const auto& consequence = MinigameUtils::CONSEQUENCES[rand() % 5];
    if(won) {
        if(consequence.type == MinigameUtils::Consequence::ENERGY) {
            state.playerEnergy += consequence.value;
            mvprintw(LINES/2, (COLS-30)/2, "You gained %d energy!", consequence.value);
        } else {
            state.playerHealth = std::min(state.playerHealth + consequence.value, 10);
            mvprintw(LINES/2, (COLS-30)/2, "You recovered %d health!", consequence.value);
        }
    } else {
        if(consequence.type == MinigameUtils::Consequence::ENERGY) {
            state.playerEnergy = std::max(0, state.playerEnergy - consequence.value);
            mvprintw(LINES/2, (COLS-30)/2, "You lost %d energy!", consequence.value);
        } else {
            state.playerHealth -= consequence.value;
            mvprintw(LINES/2, (COLS-30)/2, "You lost %d health!", consequence.value);
        }
    }
    refresh();
    napms(1500);
}

void MinigameUtils::renderCard(int y, int x, int rank, int suit, bool isAscii) {
    // Simple ASCII card display
    const char* cardArt[] = {
        "+-----+",
        "|%2s%2s|",  // Rank and suit will go here
        "|     |",
        "|%2s%2s|",  // Suit and rank mirrored
        "+-----+"
    };

    // Use basic ASCII suits
    const char* suitChars[] = {"^", "v", "<>", "o"};  // Spades, Hearts, Diamonds, Clubs
    
    init_pair(15, COLOR_RED, COLOR_BLACK);
    
    // Draw card frame
    for(int i = 0; i < 5; i++) {
        if(i == 1) {
            if(suit == 1 || suit == 2) attron(COLOR_PAIR(15));
            mvprintw(y+i, x, cardArt[i], ranks[rank], suitChars[suit]);
            if(suit == 1 || suit == 2) attroff(COLOR_PAIR(15));
        }
        else if(i == 3) {
            if(suit == 1 || suit == 2) attron(COLOR_PAIR(15));
            mvprintw(y+i, x, cardArt[i], suitChars[suit], ranks[rank]);
            if(suit == 1 || suit == 2) attroff(COLOR_PAIR(15));
        } else {
            mvprintw(y+i, x, cardArt[i]);
        }
    }
}