#include "game.h"

void Game::increaseTensorGauge(int amount) {
    int oldValue = state.tensor.current;
    state.tensor.current += amount;
    
    // Show gauge increase with pause
    state.printGameState();
    mvprintw(LINES-1, 2, "Tensor gauge increased: %d → %d", oldValue, state.tensor.current);
    refresh();
    napms(500);
    
    // Check if tensor peaked
    if (state.tensor.current >= state.tensor.maximum) {
        handleTensorPeak();
    }
}

void Game::handleTensorPeak() {
    clear();
    box(stdscr, 0, 0);
    
    mvprintw(LINES/2 - 2, (COLS-20)/2, "Tensor Peak!");
    mvprintw(LINES/2 - 1, (COLS-40)/2, "Initiating minigame sequence...");
    refresh();
    napms(1000);
    
    // Randomly select and play a minigame
    bool result;
    int gameChoice = rand() % 5;
    switch (gameChoice) {
        case 0: result = playCoinToss(); break;
        case 1: result = playHighLow(); break;
        case 2: result = playRoulette(); break;
        case 3: result = playDiceRoll(); break;
        case 4: result = playRPS(); break;
    }
    
    // Apply consequence and reset gauge
    applyMinigameConsequence(result);
    resetTensorGauge();
}

void Game::resetTensorGauge() {
    state.tensor.current = 0;
    if (state.tensor.maximum < state.tensor.ABSOLUTE_MAX) {
        state.tensor.maximum++;
        mvprintw(LINES/2, (COLS-40)/2, "Tensor maximum increased to %d!", state.tensor.maximum);
    } else {
        mvprintw(LINES/2, (COLS-40)/2, "Tensor maximum is at cap (%d)", state.tensor.ABSOLUTE_MAX);
    }
    refresh();
    napms(1000);
}
