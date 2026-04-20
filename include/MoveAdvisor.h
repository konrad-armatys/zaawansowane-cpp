#ifndef MOVEADVISOR_H
#define MOVEADVISOR_H

#include "Board.h"
#include "IHeuristic.h"
#include "PuzzleEngine.h"
#include <memory>
#include <vector>
#include <optional>
#include <limits>

/**
 * @brief Struktura reprezentująca ścieżkę ruchów wraz z jej oceną
 */
struct MovePath {
    std::vector<Direction> moves;
    double finalScore;

    /**
     * @brief Operator porównania dla sortowania (malejąco po score)
     * @param other Inna ścieżka do porównania
     * @return true jeśli ta ścieżka ma niższy score (lepszy)
     */
    [[nodiscard]] bool operator<(const MovePath& other) const noexcept {
        return finalScore < other.finalScore;
    }
};

/**
 * @brief Klasa implementująca inteligentnego asystenta (Look-ahead depth 4)
 *
 * Symuluje wszystkie możliwe sekwencje 4 ruchów i wybiera najlepszy
 * pierwszy ruch zgodnie z wybraną heurystyką.
 */
class MoveAdvisor {
private:
    std::shared_ptr<IHeuristic> heuristic_;
    static constexpr int LOOKAHEAD_DEPTH = 4;

    /**
     * @brief Oblicza wszystkie możliwe kierunki ruchu dla danej pozycji pustego pola
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @param boardSize Rozmiar planszy
     * @return Wektor możliwych kierunków ruchu
     */
    [[nodiscard]] std::vector<Direction> getPossibleMoves(int emptyX, int emptyY, int boardSize) const;

    /**
     * @brief Oblicza nową pozycję pustego pola po ruchu w danym kierunku
     * @param emptyX Obecna współrzędna x pustego pola
     * @param emptyY Obecna współrzędna y pustego pola
     * @param dir Kierunek ruchu
     * @return Para współrzędnych nowej pozycji
     */
    [[nodiscard]] std::pair<int, int> getNewEmptyPosition(int emptyX, int emptyY, Direction dir) const noexcept;

    /**
     * @brief Symuluje ruch i zwraca wynikową planszę oraz nową pozycję pustego pola
     * @param board Aktualna plansza
     * @param dir Kierunek ruchu
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @return Para: nowa plansza i para współrzędnych nowej pozycji pustego pola
     */
    [[nodiscard]] std::pair<Board<int>, std::pair<int, int>> simulateMove(
        const Board<int>& board, Direction dir, int emptyX, int emptyY) const;

    /**
     * @brief Rekurencyjnie eksploruje wszystkie możliwe sekwencje ruchów
     * @param board Aktualna plansza
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @param depth Pozostała głębokość do eksploracji
     * @param currentPath Obecna ścieżka ruchów
     * @param lastMove Ostatni wykonany ruch (aby uniknąć cofania)
     * @return Najlepsza ścieżka z tej pozycji
     */
    [[nodiscard]] MovePath explorePaths(
        const Board<int>& board, int emptyX, int emptyY, int depth,
        std::vector<Direction> currentPath, std::optional<Direction> lastMove) const;

    /**
     * @brief Sprawdza czy ruch jest odwrotnością poprzedniego (cofanie)
     * @param move Ruch do sprawdzenia
     * @param lastMove Poprzedni ruch
     * @return true jeśli ruchy są przeciwne
     */
    [[nodiscard]] bool isReverseMove(Direction move, Direction lastMove) const noexcept;

public:
    /**
     * @brief Konstruktor przyjmujący heurystykę
     * @param heuristic Wskaźnik do obiektu heurystyki
     * @throws std::invalid_argument Gdy heuristic jest nullptr
     */
    explicit MoveAdvisor(std::shared_ptr<IHeuristic> heuristic);

    /**
     * @brief Sugeruje najlepszy ruch na podstawie symulacji 4 kroków w przód
     * @param currentBoard Aktualna plansza
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @return Sugerowany kierunek ruchu lub std::nullopt jeśli brak możliwych ruchów
     */
    [[nodiscard]] std::optional<Direction> suggestMove(
        const Board<int>& currentBoard, int emptyX, int emptyY) const;

    /**
     * @brief Zmienia używaną heurystykę
     * @param heuristic Nowa heurystyka
     * @throws std::invalid_argument Gdy heuristic jest nullptr
     */
    void setHeuristic(std::shared_ptr<IHeuristic> heuristic);

    /**
     * @brief Pobiera głębokość analizy
     * @return Głębokość look-ahead (zawsze 4)
     */
    [[nodiscard]] constexpr int getDepth() const noexcept {
        return LOOKAHEAD_DEPTH;
    }
};

#endif
