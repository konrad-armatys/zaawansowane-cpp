#include "Board.h"
#include "ManhattanDistance.h"
#include <iostream>
#include <iomanip>

void printBoard(const Board<int>& board) {
    const int size = board.getSize();
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int value = board.at(x, y);
            if (value == 0) {
                std::cout << std::setw(3) << ".";
            } else {
                std::cout << std::setw(3) << value;
            }
        }
        std::cout << '\n';
    }
}

int main() {
    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 8, 0
    });

    std::cout << "Board:\n";
    printBoard(board);

    auto heuristic = std::make_shared<ManhattanDistance>();
    std::cout << "\nManhattan Distance: " << heuristic->calculate(board) << "\n";

    std::cout << "\nEmpty position: (2, 2)\n";
    std::cout << "This is already the SOLVED board!\n";

    return 0;
}
