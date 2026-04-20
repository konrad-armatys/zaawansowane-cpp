#include "LookAhead.h"
#include <algorithm>

LookAhead::LookAhead(std::shared_ptr<IHeuristic> heuristic)
    : heuristic_(std::move(heuristic)) {
    if (!heuristic_) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
}

std::vector<Direction> LookAhead::getPossibleMoves(int emptyX, int emptyY, int boardSize) const {
    std::vector<Direction> moves;
    moves.reserve(4);

    if (emptyY > 0) moves.push_back(Direction::Up);
    if (emptyY < boardSize - 1) moves.push_back(Direction::Down);
    if (emptyX > 0) moves.push_back(Direction::Left);
    if (emptyX < boardSize - 1) moves.push_back(Direction::Right);

    return moves;
}

std::pair<int, int> LookAhead::getNewEmptyPosition(int emptyX, int emptyY, Direction dir) const noexcept {
    switch (dir) {
        case Direction::Up:    return {emptyX, emptyY - 1};
        case Direction::Down:  return {emptyX, emptyY + 1};
        case Direction::Left:  return {emptyX - 1, emptyY};
        case Direction::Right: return {emptyX + 1, emptyY};
    }
    return {emptyX, emptyY};
}

Board<int> LookAhead::simulateMove(const Board<int>& board, Direction dir, int emptyX, int emptyY) const {
    Board<int> newBoard = board.getCopy();
    const auto [newEmptyX, newEmptyY] = getNewEmptyPosition(emptyX, emptyY, dir);
    newBoard.swap(emptyX, emptyY, newEmptyX, newEmptyY);
    return newBoard;
}

std::vector<MoveEvaluation> LookAhead::evaluateMoves(const Board<int>& board, int emptyX, int emptyY) const {
    const std::vector<Direction> possibleMoves = getPossibleMoves(emptyX, emptyY, board.getSize());
    std::vector<MoveEvaluation> evaluations;
    evaluations.reserve(possibleMoves.size());

    for (const Direction dir : possibleMoves) {
        const Board<int> resultBoard = simulateMove(board, dir, emptyX, emptyY);
        const double score = heuristic_->calculate(resultBoard);
        evaluations.push_back({dir, score});
    }


    std::sort(evaluations.begin(), evaluations.end());

    return evaluations;
}

std::optional<Direction> LookAhead::getBestMove(const Board<int>& board, int emptyX, int emptyY) const {
    const std::vector<MoveEvaluation> evaluations = evaluateMoves(board, emptyX, emptyY);

    if (evaluations.empty()) {
        return std::nullopt;
    }

    return evaluations.front().direction;
}

void LookAhead::setHeuristic(std::shared_ptr<IHeuristic> heuristic) {
    if (!heuristic) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
    heuristic_ = std::move(heuristic);
}
