#include "game.h"

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    int choice = 0;
    const char* menu[] = {"Start Game", "Exit"};
    
    while(true) {
        clear();
        box(stdscr, 0, 0);
        
        // Title with box
        mvprintw(3, (COLS-16)/2, "Tensor Concord");
        mvhline(4, (COLS-30)/2, ACS_HLINE, 30);
        
        // Menu options centered
        for(int i = 0; i < 2; i++) {
            if(i == choice) attron(A_REVERSE);
            mvprintw(i+6, (COLS-10)/2, "%s", menu[i]);
            if(i == choice) attroff(A_REVERSE);
        }
        
        // Navigation hint at bottom
        mvprintw(LINES-2, 2, "Use Up/Down arrows to select, Enter to confirm");
        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP: choice = 0; break;
            case KEY_DOWN: choice = 1; break;
            case '\n':
                if(choice == 0) {
                    clear();
                    Game game;
                    game.run();
                } else {
                    endwin();
                    return 0;
                }
                break;
        }
    }
}