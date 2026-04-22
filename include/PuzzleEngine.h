#ifndef PUZZLEENGINE_H
#define PUZZLEENGINE_H

#include "Board.h"
#include "GameStats.h"
#include <random>
#include <optional>

/**
 * @brief Kierunki ruchu w grze N-Puzzle
 */
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

/**
 * @brief Klasa zarządzająca logiką gry N-Puzzle
 * @tparam T Typ elementów na planszy
 */
template <typename T>
requires std::copyable<T>
class PuzzleEngine {
private:
    Board<T> board_;
    GameStats stats_;
    std::mt19937 rng_;

    int emptyX_;
    int emptyY_;

    /**
     * @brief Znajduje pozycję pustego pola na planszy
     * @param emptyValue Wartość reprezentująca puste pole (zwykle 0)
     * @return Para współrzędnych (x, y) pustego pola
     * @throws std::runtime_error Gdy puste pole nie zostało znalezione
     */
    std::pair<int, int> findEmptyTile(const T& emptyValue) const;

    /**
     * @brief Oblicza liczbę inwersji w aktualnym układzie planszy
     * @param emptyValue Wartość reprezentująca puste pole
     * @return Liczba inwersji
     */
    int countInversions(const T& emptyValue) const;

    /**
     * @brief Sprawdza rozwiązywalność układu planszy
     * @param emptyValue Wartość reprezentująca puste pole
     * @return true jeśli układ jest rozwiązywalny, false w przeciwnym wypadku
     */
    bool isSolvable(const T& emptyValue) const;

    /**
     * @brief Naprawia nierozwiązywalny układ zamieniając dwa ostatnie niezerowe kafelki
     * @param emptyValue Wartość reprezentująca puste pole
     */
    void fixUnsolvableLayout(const T& emptyValue);

    /**
     * @brief Sprawdza czy ruch w danym kierunku jest możliwy
     * @param dir Kierunek ruchu
     * @return true jeśli ruch jest możliwy, false w przeciwnym wypadku
     */
    bool isMovePossible(Direction dir) const;

    /**
     * @brief Wykonuje ruch pustego pola w danym kierunku
     * @param dir Kierunek ruchu
     * @param emptyValue Wartość reprezentująca puste pole
     * @return true jeśli ruch został wykonany, false w przeciwnym wypadku
     */
    bool executeMove(Direction dir, const T& emptyValue);

public:
    /**
     * @brief Konstruktor tworzący silnik gry
     * @param size Rozmiar planszy (N x N)
     */
    explicit PuzzleEngine(int size);

    /**
     * @brief Konstruktor odtwarzający stan gry
     * @param board Plansza do przywrócenia
     * @param stats Statystyki do przywrócenia
     * @param emptyValue Wartość reprezentująca puste pole
     */
    PuzzleEngine(const Board<T>& board, const GameStats& stats, const T& emptyValue = T{});

    /**
     * @brief Tasuje planszę generując losowy rozwiązywalny układ
     * @param emptyValue Wartość reprezentująca puste pole (zwykle 0)
     */
    void shuffle(const T& emptyValue = T{});

    /**
     * @brief Próbuje wykonać ruch w danym kierunku
     * @param dir Kierunek ruchu
     * @param emptyValue Wartość reprezentująca puste pole
     * @return true jeśli ruch został wykonany, false w przeciwnym wypadku
     */
    bool move(Direction dir, const T& emptyValue = T{});

    /**
     * @brief Sprawdza czy plansza jest w stanie rozwiązanym
     * @param emptyValue Wartość reprezentująca puste pole
     * @return true jeśli gra została rozwiązana, false w przeciwnym wypadku
     */
    bool isSolved(const T& emptyValue = T{}) const;

    /**
     * @brief Pobiera aktualną planszę
     * @return Stała referencja do planszy
     */
    const Board<T>& getBoard() const noexcept;

    /**
     * @brief Pobiera statystyki gry
     * @return Stała referencja do statystyk
     */
    const GameStats& getStats() const noexcept;

    /**
     * @brief Pobiera statystyki gry (wersja non-const)
     * @return Referencja do statystyk
     */
    GameStats& getStats() noexcept;

    /**
     * @brief Pobiera pozycję pustego pola
     * @return Para współrzędnych (x, y)
     */
    std::pair<int, int> getEmptyPosition() const noexcept;

    /**
     * @brief Aktualizuje pozycję pustego pola na podstawie bieżącego stanu planszy
     * @param emptyValue Wartość reprezentująca puste pole
     * @throws std::runtime_error Gdy puste pole nie zostało znalezione
     */
    void updateEmptyPosition(const T& emptyValue = T{});

    /**
     * @brief Zamienia miejscami dwa kafelki na planszy
     * @param x1 Współrzędna X pierwszego kafelka
     * @param y1 Współrzędna Y pierwszego kafelka
     * @param x2 Współrzędna X drugiego kafelka
     * @param y2 Współrzędna Y drugiego kafelka
     * @throws std::out_of_range Gdy współrzędne są poza planszą
     */
    void swapTiles(int x1, int y1, int x2, int y2);
};

#include "PuzzleEngine.tpp"

#endif
