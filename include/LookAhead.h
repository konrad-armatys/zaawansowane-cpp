#ifndef LOOKAHEAD_H
#define LOOKAHEAD_H

#include "Board.h"
#include "IHeuristic.h"
#include "PuzzleEngine.h"
#include <vector>
#include <memory>
#include <optional>

/**
 * @brief Struktura reprezentująca ocenę możliwego ruchu
 */
struct MoveEvaluation {
    Direction direction;
    double score;

    /**
     * @brief Operator porównania dla sortowania (malejąco po score)
     * @param other Inna ocena do porównania
     * @return true jeśli ten ruch ma niższy score (lepszy)
     */
    [[nodiscard]] bool operator<(const MoveEvaluation& other) const noexcept {
        return score < other.score;
    }
};

/**
 * @brief Klasa implementująca system podpowiedzi Look-ahead
 *
 * Analizuje możliwe ruchy i ocenia je za pomocą wybranej heurystyki,
 * wskazując najlepszy ruch dla gracza.
 *
 * @tparam T Typ elementu planszy, musi spełniać PuzzleTile concept.
 *           Musi pasować do typu używanego w IHeuristic<T>.
 */
template<PuzzleTile T>
class LookAhead {
private:
    std::shared_ptr<IHeuristic<T>> heuristic_;

    /**
     * @brief Oblicza wszystkie możliwe kierunki ruchu dla danej pozycji pustego pola
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @param boardSize Rozmiar planszy
     * @return Wektor możliwych kierunków ruchu
     */
    [[nodiscard]] std::vector<Direction> getPossibleMoves(int emptyX, int emptyY, int boardSize) const;

    /**
     * @brief Symuluje ruch i zwraca wynikową planszę
     * @param board Aktualna plansza
     * @param dir Kierunek ruchu
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @return Nowa plansza po wykonaniu ruchu
     */
    [[nodiscard]] Board<T> simulateMove(const Board<T>& board, Direction dir, int emptyX, int emptyY) const;

    /**
     * @brief Oblicza nową pozycję pustego pola po ruchu w danym kierunku
     * @param emptyX Obecna współrzędna x pustego pola
     * @param emptyY Obecna współrzędna y pustego pola
     * @param dir Kierunek ruchu
     * @return Para współrzędnych nowej pozycji
     */
    [[nodiscard]] std::pair<int, int> getNewEmptyPosition(int emptyX, int emptyY, Direction dir) const noexcept;

public:
    /**
     * @brief Konstruktor przyjmujący heurystykę
     * @param heuristic Wskaźnik do obiektu heurystyki
     */
    explicit LookAhead(std::shared_ptr<IHeuristic<T>> heuristic);

    /**
     * @brief Analizuje możliwe ruchy i zwraca je posortowane od najlepszego
     * @param board Aktualna plansza
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @return Wektor ocen ruchów posortowany od najlepszego
     */
    [[nodiscard]] std::vector<MoveEvaluation> evaluateMoves(const Board<T>& board, int emptyX, int emptyY) const;

    /**
     * @brief Zwraca najlepszy możliwy ruch
     * @param board Aktualna plansza
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @return Najlepszy kierunek ruchu lub std::nullopt jeśli brak możliwych ruchów
     */
    [[nodiscard]] std::optional<Direction> getBestMove(const Board<T>& board, int emptyX, int emptyY) const;

    /**
     * @brief Zmienia używaną heurystykę
     * @param heuristic Nowa heurystyka
     */
    void setHeuristic(std::shared_ptr<IHeuristic<T>> heuristic);
};

#include "LookAhead.tpp"

#endif
