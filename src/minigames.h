#pragma once
#include "game.h"

namespace MinigameUtils {
    struct Consequence {
        enum Type {
            HEALTH,
            ENERGY
        } type;
        int value;
        const char* description;
    };

    // Define consequences for each minigame
    constexpr std::array<Consequence, 5> CONSEQUENCES = {{
        {Consequence::ENERGY, 2, "Stake: 2 Energy"},
        {Consequence::HEALTH, 1, "Stake: 1 Health"},
        {Consequence::ENERGY, 3, "Stake: 3 Energy"},
        {Consequence::HEALTH, 2, "Stake: 2 Health"},
        {Consequence::ENERGY, 1, "Stake: 1 Energy"}
    }};

    // Add fallback art
    struct CardArt {
        static constexpr const char* suits[2][4] = {
            {"S", "H", "D", "C"},            // ASCII
            {"\x06", "\x03", "\x04", "\x05"} // Extended ASCII
        };

        static constexpr const char* borders[2][3] = {
            {"+-------+", "|       |", "+-------+"},   // ASCII
            {"┌───────┐", "│       │", "└───────┘"}    // Extended ASCII
        };
    };

    // Add these alternative art sets
    struct AsciiArt {
        static constexpr const char* cardBorders[3] = {
            "+-------+",
            "|       |",
            "+-------+"
        };
        
        static constexpr const char* suits[4] = {
            "S", "H", "D", "C"  // Simple ASCII suits
        };

        static constexpr const char* rouletteBall = "o";  // Instead of •
        static constexpr const char* diceArt[6] = {
            // Simple ASCII versions
            "[1]", "[2]", "[3]", "[4]", "[5]", "[6]"
        };

        // Simplified box drawing characters
        static constexpr const char* boxChars[6] = {
            "+", "-", "|", "+", "+", "+"  // top, horiz, vert, topleft, topright, bottom
        };
    };

    // Move drawCard into MinigameUtils namespace and rename for clarity
    void renderCard(int y, int x, int rank, int suit, bool isAscii = false);

    inline void drawMinigameFrame(const char* title, const std::string& description = "") {
        clear();
        box(stdscr, 0, 0);
        
        // Draw centered title with decoration
        std::string titleDecor = "=== " + std::string(title) + " ===";
        mvprintw(2, (COLS-titleDecor.length())/2, "%s", titleDecor.c_str());
        
        // Draw optional description
        if(!description.empty()) {
            mvprintw(4, (COLS-description.length())/2, "%s", description.c_str());
        }
        
        // Add navigation hint
        mvprintw(LINES-2, (COLS-35)/2, "UP/DOWN to select, ENTER to confirm");
        refresh();
    }

    inline void drawMinigameResult(bool won, const char* art = nullptr) {
        clear();
        box(stdscr, 0, 0);
        
        // Draw result
        std::string result = won ? "YOU WIN!" : "YOU LOSE!";
        mvprintw(LINES/2-1, (COLS-result.length())/2, "%s", result.c_str());
        
        // Draw optional art
        if(art) {
            std::istringstream iss(art);
            std::string line;
            int y = LINES/2 + 1;
            while(std::getline(iss, line)) {
                mvprintw(y++, (COLS-line.length())/2, "%s", line.c_str());
            }
        }
        
        mvprintw(LINES-2, (COLS-30)/2, "Press any key to continue...");
        refresh();
        napms(1500);
        getch();
    }
}

// Remove the duplicate playMinigameMenu() implementation and keep only:
void Game::playMinigameMenu() {
    std::vector<std::string> options = {
        "Coin Toss",
        "High-Low Cards",
        "Roulette",
        "Dice Roll",
        "Rock Paper Scissors",
        "Return to Menu"
    };
    
    bool running = true;
    while(running) {
        int choice = showMenu(options, "Practice Games");
        
        if(choice >= 0 && choice < 5) {
            bool result = false;
            switch(choice) {
                case 0: result = playCoinToss(); break;
                case 1: result = playHighLow(); break;
                case 2: result = playRoulette(); break;
                case 3: result = playDiceRoll(); break;
                case 4: result = playRPS(); break;
            }
            MinigameUtils::drawMinigameResult(result);
        }
        else if(choice == 5 || choice == -1) {
            running = false;
        }
    }
}