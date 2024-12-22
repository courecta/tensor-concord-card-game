#include "game.h"
#include <iostream>

int main() {
    try {
        std::cout << "Welcome to Tensor Concord (Terminal Version)\n"
                  << "===========================================\n\n";
        
        Game game;
        game.run();
        
        return 0;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}