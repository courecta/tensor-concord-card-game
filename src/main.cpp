#include "game.h"

int main() {
    // Initialize curses
    initscr();
    start_color();
    use_default_colors();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Initialize game with colors
    GameUI::initializeAllColors();
    Game game;
    game.run();
    
    // Clean up
    endwin();
    return 0;
}