#ifndef LOOKAHEAD_TPP
#define LOOKAHEAD_TPP

#include "LookAhead.h"
#include <algorithm>

template<PuzzleTile T>
LookAhead<T>::LookAhead(std::shared_ptr<IHeuristic<T>> heuristic)
    : heuristic_(std::move(heuristic)) {
    if (!heuristic_) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
}

template<PuzzleTile T>
std::vector<Direction> LookAhead<T>::getPossibleMoves(int emptyX, int emptyY, int boardSize) const {
    std::vector<Direction> moves;
    moves.reserve(4);

    if (emptyY > 0) moves.push_back(Direction::Up);
    if (emptyY < boardSize - 1) moves.push_back(Direction::Down);
    if (emptyX > 0) moves.push_back(Direction::Left);
    if (emptyX < boardSize - 1) moves.push_back(Direction::Right);

    return moves;
}

template<PuzzleTile T>
std::pair<int, int> LookAhead<T>::getNewEmptyPosition(int emptyX, int emptyY, Direction dir) const noexcept {
    switch (dir) {
        case Direction::Up:    return {emptyX, emptyY - 1};
        case Direction::Down:  return {emptyX, emptyY + 1};
        case Direction::Left:  return {emptyX - 1, emptyY};
        case Direction::Right: return {emptyX + 1, emptyY};
    }
    return {emptyX, emptyY};
}

template<PuzzleTile T>
Board<T> LookAhead<T>::simulateMove(const Board<T>& board, Direction dir, int emptyX, int emptyY) const {
    Board<T> newBoard = board.getCopy();
    const auto [newEmptyX, newEmptyY] = getNewEmptyPosition(emptyX, emptyY, dir);
    newBoard.swap(emptyX, emptyY, newEmptyX, newEmptyY);
    return newBoard;
}

template<PuzzleTile T>
std::vector<MoveEvaluation> LookAhead<T>::evaluateMoves(const Board<T>& board, int emptyX, int emptyY) const {
    const std::vector<Direction> possibleMoves = getPossibleMoves(emptyX, emptyY, board.getSize());
    std::vector<MoveEvaluation> evaluations;
    evaluations.reserve(possibleMoves.size());

    for (const Direction dir : possibleMoves) {
        const Board<T> resultBoard = simulateMove(board, dir, emptyX, emptyY);
        const double score = heuristic_->calculate(resultBoard);
        evaluations.push_back({dir, score});
    }

    std::sort(evaluations.begin(), evaluations.end());

    return evaluations;
}

template<PuzzleTile T>
std::optional<Direction> LookAhead<T>::getBestMove(const Board<T>& board, int emptyX, int emptyY) const {
    const std::vector<MoveEvaluation> evaluations = evaluateMoves(board, emptyX, emptyY);

    if (evaluations.empty()) {
        return std::nullopt;
    }

    return evaluations.front().direction;
}

template<PuzzleTile T>
void LookAhead<T>::setHeuristic(std::shared_ptr<IHeuristic<T>> heuristic) {
    if (!heuristic) {
        throw std::invalid_argument("Heuristic cannot be null");
    }
    heuristic_ = std::move(heuristic);
}

#endif // LOOKAHEAD_TPP
