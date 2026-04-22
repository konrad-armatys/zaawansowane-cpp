#include "Board.h"
#include "ManhattanDistance.h"
#include "PuzzleEngine.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <memory>

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

std::string dirToString(Direction dir) {
    switch (dir) {
        case Direction::Up:    return "Up";
        case Direction::Down:  return "Down";
        case Direction::Left:  return "Left";
        case Direction::Right: return "Right";
    }
    return "Unknown";
}

std::pair<int, int> getNewEmptyPos(int emptyX, int emptyY, Direction dir) {
    switch (dir) {
        case Direction::Up:    return {emptyX, emptyY - 1};
        case Direction::Down:  return {emptyX, emptyY + 1};
        case Direction::Left:  return {emptyX - 1, emptyY};
        case Direction::Right: return {emptyX + 1, emptyY};
    }
    return {emptyX, emptyY};
}

Board<int> applyMove(const Board<int>& board, Direction dir, int emptyX, int emptyY) {
    Board<int> newBoard = board.getCopy();
    auto [newX, newY] = getNewEmptyPos(emptyX, emptyY, dir);
    newBoard.swap(emptyX, emptyY, newX, newY);
    return newBoard;
}

std::vector<Direction> getPossibleMoves(int emptyX, int emptyY, int boardSize) {
    std::vector<Direction> moves;
    if (emptyY > 0) moves.push_back(Direction::Up);
    if (emptyY < boardSize - 1) moves.push_back(Direction::Down);
    if (emptyX > 0) moves.push_back(Direction::Left);
    if (emptyX < boardSize - 1) moves.push_back(Direction::Right);
    return moves;
}

struct PathResult {
    std::vector<Direction> path;
    double score;
};

PathResult exploreManual(
    const Board<int>& board, int emptyX, int emptyY, int depth,
    std::vector<Direction> currentPath,
    const std::shared_ptr<ManhattanDistance<int>>& heuristic,
    int indent = 0) {

    std::string indentStr(indent * 2, ' ');

    if (depth == 0) {
        double score = heuristic->calculate(board);
        std::cout << indentStr << "Leaf reached. Score: " << score << ", Path: ";
        for (auto d : currentPath) std::cout << dirToString(d) << " ";
        std::cout << "\n";
        return PathResult{currentPath, score};
    }

    auto possibleMoves = getPossibleMoves(emptyX, emptyY, board.getSize());

    PathResult best{currentPath, std::numeric_limits<double>::max()};

    for (auto move : possibleMoves) {
        std::cout << indentStr << "Trying " << dirToString(move) << " at depth " << depth << "\n";

        auto newBoard = applyMove(board, move, emptyX, emptyY);
        auto [newX, newY] = getNewEmptyPos(emptyX, emptyY, move);

        std::vector<Direction> newPath = currentPath;
        newPath.push_back(move);

        auto result = exploreManual(newBoard, newX, newY, depth - 1, newPath, heuristic, indent + 1);

        std::cout << indentStr << "Result for " << dirToString(move) << ": " << result.score << "\n";

        if (result.score < best.score) {
            best = result;
        }
    }

    return best;
}

int main() {
    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 0, 8
    });

    std::cout << "Initial board:\n";
    printBoard(board);
    std::cout << "\n";

    auto heuristic = std::make_shared<ManhattanDistance<int>>();

    int emptyX = 1, emptyY = 2;

    std::cout << "=== Manual exploration with depth 2 ===\n\n";
    auto result = exploreManual(board, emptyX, emptyY, 2, {}, heuristic);

    std::cout << "\n=== Best path found ===\n";
    std::cout << "Score: " << result.score << "\n";
    std::cout << "Path: ";
    for (auto d : result.path) {
        std::cout << dirToString(d) << " ";
    }
    std::cout << "\n";

    if (!result.path.empty()) {
        std::cout << "First move: " << dirToString(result.path[0]) << "\n";
    }

    return 0;
}
