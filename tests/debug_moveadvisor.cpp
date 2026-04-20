#include "Board.h"
#include "MoveAdvisor.h"
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
    std::cout << '\n';
}

std::string dirToString(Direction dir) {
    switch (dir) {
        case Direction::Up:    return "Up";
        case Direction::Down:  return "Down";
        case Direction::Left:  return "Left";
        case Direction::Right: return "Right";
    }
    return "Unknown";
}

Board<int> applyMove(const Board<int>& board, Direction dir, int emptyX, int emptyY) {
    Board<int> newBoard = board.getCopy();
    int newX = emptyX, newY = emptyY;

    switch (dir) {
        case Direction::Up:    newY--; break;
        case Direction::Down:  newY++; break;
        case Direction::Left:  newX--; break;
        case Direction::Right: newX++; break;
    }

    newBoard.swap(emptyX, emptyY, newX, newY);
    return newBoard;
}

int main() {
    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 0, 8
    });

    std::cout << "Initial board (needs 1 move to solve):\n";
    printBoard(board);

    auto heuristic = std::make_shared<ManhattanDistance>();

    int emptyX = 1, emptyY = 2;

    std::cout << "Testing all possible moves:\n\n";


    if (emptyY > 0) {
        Board<int> afterUp = applyMove(board, Direction::Up, emptyX, emptyY);
        std::cout << "After Up:\n";
        printBoard(afterUp);
        std::cout << "Manhattan Distance: " << heuristic->calculate(afterUp) << "\n\n";
    }


    if (emptyY < 2) {
        Board<int> afterDown = applyMove(board, Direction::Down, emptyX, emptyY);
        std::cout << "After Down:\n";
        printBoard(afterDown);
        std::cout << "Manhattan Distance: " << heuristic->calculate(afterDown) << "\n\n";
    }


    if (emptyX > 0) {
        Board<int> afterLeft = applyMove(board, Direction::Left, emptyX, emptyY);
        std::cout << "After Left:\n";
        printBoard(afterLeft);
        std::cout << "Manhattan Distance: " << heuristic->calculate(afterLeft) << "\n\n";
    }


    if (emptyX < 2) {
        Board<int> afterRight = applyMove(board, Direction::Right, emptyX, emptyY);
        std::cout << "After Right:\n";
        printBoard(afterRight);
        std::cout << "Manhattan Distance: " << heuristic->calculate(afterRight) << "\n\n";
    }

    std::cout << "=== MoveAdvisor suggestion ===\n";
    MoveAdvisor advisor(heuristic);
    auto suggestion = advisor.suggestMove(board, emptyX, emptyY);

    if (suggestion.has_value()) {
        std::cout << "Suggested: " << dirToString(suggestion.value()) << "\n";
    }

    return 0;
}
