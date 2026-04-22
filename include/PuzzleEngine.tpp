#include "PuzzleEngine.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

template <typename T>
requires std::copyable<T>
PuzzleEngine<T>::PuzzleEngine(int size)
    : board_(size),
      stats_(size),
      rng_(std::random_device{}()),
      emptyX_(size - 1),
      emptyY_(size - 1)
{
    if (size < 2) {
        throw std::invalid_argument("Puzzle size must be at least 2x2");
    }
}

template <typename T>
requires std::copyable<T>
PuzzleEngine<T>::PuzzleEngine(const Board<T>& board, const GameStats& stats, const T& emptyValue)
    : board_(board),
      stats_(stats),
      rng_(std::random_device{}())
{
    auto [x, y] = findEmptyTile(emptyValue);
    emptyX_ = x;
    emptyY_ = y;
}

template <typename T>
requires std::copyable<T>
std::pair<int, int> PuzzleEngine<T>::findEmptyTile(const T& emptyValue) const {
    const int size = board_.getSize();
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (board_.at(x, y) == emptyValue) {
                return {x, y};
            }
        }
    }
    throw std::runtime_error("Empty tile not found on board");
}

template <typename T>
requires std::copyable<T>
int PuzzleEngine<T>::countInversions(const T& emptyValue) const {
    const auto& data = board_.getData();
    int inversions = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == emptyValue) continue;

        for (size_t j = i + 1; j < data.size(); ++j) {
            if (data[j] == emptyValue) continue;

            if (data[i] > data[j]) {
                ++inversions;
            }
        }
    }

    return inversions;
}

template <typename T>
requires std::copyable<T>
bool PuzzleEngine<T>::isSolvable(const T& emptyValue) const {
    const int size = board_.getSize();
    const int inversions = countInversions(emptyValue);

    if (size % 2 == 1) {
        // Dla N nieparzystych: liczba inwersji musi być parzysta
        return inversions % 2 == 0;
    } else {
        // Dla N parzystych: (inwersje + rząd pustego pola od dołu) musi być nieparzyste
        auto [emptyX, emptyY] = findEmptyTile(emptyValue);
        int emptyRowFromBottom = size - emptyY;
        return (inversions + emptyRowFromBottom) % 2 == 1;
    }
}

template <typename T>
requires std::copyable<T>
void PuzzleEngine<T>::fixUnsolvableLayout(const T& emptyValue) {
    const int size = board_.getSize();

    // Znajdź dwa ostatnie niezerowe kafelki
    int firstX = -1, firstY = -1;
    int secondX = -1, secondY = -1;

    for (int y = size - 1; y >= 0; --y) {
        for (int x = size - 1; x >= 0; --x) {
            if (board_.at(x, y) != emptyValue) {
                if (secondX == -1) {
                    secondX = x;
                    secondY = y;
                } else if (firstX == -1) {
                    firstX = x;
                    firstY = y;
                    break;
                }
            }
        }
        if (firstX != -1) break;
    }

    // Zamień miejscami dwa znalezione kafelki
    if (firstX != -1 && secondX != -1) {
        board_.swap(firstX, firstY, secondX, secondY);
    }
}

template <typename T>
requires std::copyable<T>
void PuzzleEngine<T>::shuffle(const T& emptyValue) {
    const int size = board_.getSize();
    const int totalTiles = size * size;

    // Generuj wektor wartości od 1 do N²-1 plus emptyValue
    std::vector<T> values;
    values.reserve(totalTiles);

    for (int i = 1; i < totalTiles; ++i) {
        values.push_back(static_cast<T>(i));
    }
    values.push_back(emptyValue);

    // Tasuj wartości
    std::shuffle(values.begin(), values.end(), rng_);

    // Wypełnij planszę wymieszanymi wartościami
    int idx = 0;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            board_.at(x, y) = values[idx++];
        }
    }

    // Sprawdź rozwiązywalność i napraw jeśli potrzeba
    if (!isSolvable(emptyValue)) {
        fixUnsolvableLayout(emptyValue);
    }

    // Zaktualizuj pozycję pustego pola
    auto [x, y] = findEmptyTile(emptyValue);
    emptyX_ = x;
    emptyY_ = y;

    // Zresetuj statystyki
    stats_.reset();
    stats_.update(board_);
}

template <typename T>
requires std::copyable<T>
bool PuzzleEngine<T>::isMovePossible(Direction dir) const {
    const int size = board_.getSize();

    switch (dir) {
        case Direction::Up:
            return emptyY_ > 0;
        case Direction::Down:
            return emptyY_ < size - 1;
        case Direction::Left:
            return emptyX_ > 0;
        case Direction::Right:
            return emptyX_ < size - 1;
    }
    return false;
}

template <typename T>
requires std::copyable<T>
bool PuzzleEngine<T>::executeMove(Direction dir, const T& emptyValue) {
    if (!isMovePossible(dir)) {
        return false;
    }

    int targetX = emptyX_;
    int targetY = emptyY_;

    switch (dir) {
        case Direction::Up:
            targetY--;
            break;
        case Direction::Down:
            targetY++;
            break;
        case Direction::Left:
            targetX--;
            break;
        case Direction::Right:
            targetX++;
            break;
    }

    board_.swap(emptyX_, emptyY_, targetX, targetY);
    emptyX_ = targetX;
    emptyY_ = targetY;

    return true;
}

template <typename T>
requires std::copyable<T>
bool PuzzleEngine<T>::move(Direction dir, const T& emptyValue) {
    if (executeMove(dir, emptyValue)) {
        ++stats_.movesCount;
        stats_.update(board_);
        return true;
    }
    return false;
}

template <typename T>
requires std::copyable<T>
bool PuzzleEngine<T>::isSolved(const T& emptyValue) const {
    const int size = board_.getSize();

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int expectedValue = y * size + x + 1;

            // Ostatnie pole powinno być puste
            if (y == size - 1 && x == size - 1) {
                if (board_.at(x, y) != emptyValue) {
                    return false;
                }
            } else {
                if (board_.at(x, y) != static_cast<T>(expectedValue)) {
                    return false;
                }
            }
        }
    }

    return true;
}

template <typename T>
requires std::copyable<T>
const Board<T>& PuzzleEngine<T>::getBoard() const noexcept {
    return board_;
}

template <typename T>
requires std::copyable<T>
const GameStats& PuzzleEngine<T>::getStats() const noexcept {
    return stats_;
}

template <typename T>
requires std::copyable<T>
GameStats& PuzzleEngine<T>::getStats() noexcept {
    return stats_;
}

template <typename T>
requires std::copyable<T>
void PuzzleEngine<T>::reset(const T& emptyValue) {
    shuffle(emptyValue);
}

template <typename T>
requires std::copyable<T>
std::pair<int, int> PuzzleEngine<T>::getEmptyPosition() const noexcept {
    return {emptyX_, emptyY_};
}
