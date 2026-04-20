#include "GameStats.h"
#include "Board.h"
#include <iostream>

int main() {
    GameStats stats(3);

    stats.onMovesChange([](int moves) {
        std::cout << "Moves changed to: " << moves << std::endl;
    });

    stats.onCorrectTilesChange([](int correct) {
        std::cout << "Correct tiles: " << correct << std::endl;
    });

    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 8, 0
    });

    std::cout << "Initial board (solved):" << std::endl;
    stats.update(board);
    std::cout << "Moves: " << stats.movesCount.get() << std::endl;

    stats.movesCount++;
    stats.movesCount++;

    board.swap(2, 2, 1, 2);
    std::cout << "\nAfter moving tile:" << std::endl;
    stats.update(board);

    stats.undoCount++;
    std::cout << "Undo count: " << stats.undoCount.get() << std::endl;

    return 0;
}
