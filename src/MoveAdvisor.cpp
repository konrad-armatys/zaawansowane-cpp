#include "MoveAdvisor.h"
#include <algorithm>

MoveAdvisor::MoveAdvisor(std::shared_ptr<IHeuristic> heuristic)
    : heuristic_(std::move(heuristic)) {
    if (!heuristic_) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
}

std::vector<Direction> MoveAdvisor::getPossibleMoves(int emptyX, int emptyY, int boardSize) const {
    std::vector<Direction> moves;
    moves.reserve(4);

    if (emptyY > 0) moves.push_back(Direction::Up);
    if (emptyY < boardSize - 1) moves.push_back(Direction::Down);
    if (emptyX > 0) moves.push_back(Direction::Left);
    if (emptyX < boardSize - 1) moves.push_back(Direction::Right);

    return moves;
}

std::pair<int, int> MoveAdvisor::getNewEmptyPosition(int emptyX, int emptyY, Direction dir) const noexcept {
    switch (dir) {
        case Direction::Up:    return {emptyX, emptyY - 1};
        case Direction::Down:  return {emptyX, emptyY + 1};
        case Direction::Left:  return {emptyX - 1, emptyY};
        case Direction::Right: return {emptyX + 1, emptyY};
    }
    return {emptyX, emptyY};
}

std::pair<Board<int>, std::pair<int, int>> MoveAdvisor::simulateMove(
    const Board<int>& board, Direction dir, int emptyX, int emptyY) const {

    Board<int> newBoard = board.getCopy();
    const auto [newEmptyX, newEmptyY] = getNewEmptyPosition(emptyX, emptyY, dir);
    newBoard.swap(emptyX, emptyY, newEmptyX, newEmptyY);

    return {std::move(newBoard), {newEmptyX, newEmptyY}};
}

bool MoveAdvisor::isReverseMove(Direction move, Direction lastMove) const noexcept {
    return (move == Direction::Up && lastMove == Direction::Down) ||
           (move == Direction::Down && lastMove == Direction::Up) ||
           (move == Direction::Left && lastMove == Direction::Right) ||
           (move == Direction::Right && lastMove == Direction::Left);
}

MovePath MoveAdvisor::explorePaths(
    const Board<int>& board, int emptyX, int emptyY, int depth,
    std::vector<Direction> currentPath, std::optional<Direction> lastMove) const {

    const double currentScore = heuristic_->calculate(board);







    if (depth == 0 || currentScore == 0.0) {
        return MovePath{
            std::move(currentPath),
            currentScore
        };
    }

    const std::vector<Direction> possibleMoves = getPossibleMoves(emptyX, emptyY, board.getSize());

    if (possibleMoves.empty()) {
        return MovePath{
            std::move(currentPath),
            std::numeric_limits<double>::max()
        };
    }

    MovePath bestPath{
        std::move(currentPath),
        std::numeric_limits<double>::max()
    };

    for (const Direction move : possibleMoves) {
        if (lastMove.has_value() && isReverseMove(move, lastMove.value())) {
            continue;
        }

        const auto [newBoard, newEmptyPos] = simulateMove(board, move, emptyX, emptyY);

        std::vector<Direction> newPath = currentPath;
        newPath.push_back(move);

        const MovePath candidatePath = explorePaths(
            newBoard,
            newEmptyPos.first,
            newEmptyPos.second,
            depth - 1,
            newPath,
            move
        );

        if (candidatePath.finalScore < bestPath.finalScore) {
            bestPath = candidatePath;
        }
    }

    return bestPath;
}

std::optional<Direction> MoveAdvisor::suggestMove(
    const Board<int>& currentBoard, int emptyX, int emptyY) const {

    const std::vector<Direction> possibleMoves = getPossibleMoves(emptyX, emptyY, currentBoard.getSize());

    if (possibleMoves.empty()) {
        return std::nullopt;
    }

    const MovePath bestPath = explorePaths(
        currentBoard,
        emptyX,
        emptyY,
        LOOKAHEAD_DEPTH,
        {},
        std::nullopt
    );

    if (bestPath.moves.empty()) {
        return std::nullopt;
    }

    return bestPath.moves.front();
}

void MoveAdvisor::setHeuristic(std::shared_ptr<IHeuristic> heuristic) {
    if (!heuristic) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
    heuristic_ = std::move(heuristic);
}
