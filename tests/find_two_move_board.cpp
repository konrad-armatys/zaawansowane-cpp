#include "Board.h"
#include "ManhattanDistance.h"
#include <iostream>
#include <iomanip>

void printBoard(const Board<int>& board) {
    const int size = board.getSize();
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int value = board.at(x, y);
            std::cout << (value == 0 ? "." : std::to_string(value)) << " ";
        }
        std::cout << '\n';
    }
}

int main() {
    auto heuristic = std::make_shared<ManhattanDistance>();

    std::cout << "=== Finding a true 2-move board ===\n\n";


    Board<int> solved(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 8, 0
    });


    Board<int> twoMoves = solved.getCopy();
    twoMoves.swap(2, 2, 1, 2);
    std::cout << "After first Left:\n";
    printBoard(twoMoves);
    std::cout << "MD: " << heuristic->calculate(twoMoves) << "\n\n";

    twoMoves.swap(1, 2, 0, 2);
    std::cout << "After second Left:\n";
    printBoard(twoMoves);
    std::cout << "MD: " << heuristic->calculate(twoMoves) << "\n";
    std::cout << "Empty at: (0, 2)\n";
    std::cout << "To solve: Right, Right\n\n";


    Board<int> twoMoves2 = solved.getCopy();
    twoMoves2.swap(2, 2, 2, 1);
    std::cout << "After Down:\n";
    printBoard(twoMoves2);
    std::cout << "MD: " << heuristic->calculate(twoMoves2) << "\n\n";

    twoMoves2.swap(2, 1, 1, 1);
    std::cout << "After Left:\n";
    printBoard(twoMoves2);
    std::cout << "MD: " << heuristic->calculate(twoMoves2) << "\n";
    std::cout << "Empty at: (1, 1)\n";

    return 0;
}
