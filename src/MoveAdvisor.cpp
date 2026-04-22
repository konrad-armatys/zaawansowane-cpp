#include "MoveAdvisor.h"
#include <algorithm>
#include <queue>
#include <map>

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

    struct State {
        Board<int> board;
        int emptyX;
        int emptyY;
        std::vector<Direction> moves;
        double score;
        std::optional<Direction> lastMove;
    };

    auto cmp = [](const State& a, const State& b) {
        return a.score > b.score;
    };

    std::priority_queue<State, std::vector<State>, decltype(cmp)> queue(cmp);
    queue.push({board, emptyX, emptyY, {}, heuristic_->calculate(board), lastMove});

    MovePath bestPath{{}, std::numeric_limits<double>::max()};

    while (!queue.empty()) {
        State current = std::move(queue.top());
        queue.pop();

        if (current.moves.size() == static_cast<size_t>(depth) || current.score == 0.0) {
            if (current.score < bestPath.finalScore) {
                bestPath = {current.moves, current.score};
            }
            continue;
        }

        const std::vector<Direction> possibleMoves = getPossibleMoves(
            current.emptyX, current.emptyY, current.board.getSize());

        for (const Direction move : possibleMoves) {
            if (current.lastMove.has_value() && isReverseMove(move, current.lastMove.value())) {
                continue;
            }

            const auto [newBoard, newEmptyPos] = simulateMove(
                current.board, move, current.emptyX, current.emptyY);

            std::vector<Direction> newMoves = current.moves;
            newMoves.push_back(move);

            double newScore = heuristic_->calculate(newBoard);

            queue.push({
                std::move(newBoard),
                newEmptyPos.first,
                newEmptyPos.second,
                std::move(newMoves),
                newScore,
                move
            });
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
