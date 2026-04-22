#ifndef MANHATTANDISTANCE_H
#define MANHATTANDISTANCE_H

#include "IHeuristic.h"

/**
 * @brief Implementacja heurystyki odległości Manhattan
 *
 * Oblicza sumę odległości Manhattan wszystkich kafelków od ich docelowych pozycji.
 * Odległość Manhattan to suma bezwzględnych różnic współrzędnych.
 *
 * @tparam T Typ elementu planszy, musi spełniać NumericTile concept.
 *           Wymagane dla operacji numerycznych (odejmowanie, konwersja do int).
 */
template<NumericTile T>
class ManhattanDistance : public IHeuristic<T> {
public:
    /**
     * @brief Oblicza sumę odległości Manhattan dla wszystkich kafelków
     * @param board Plansza do oceny
     * @return Suma odległości Manhattan wszystkich kafelków
     */
    [[nodiscard]] double calculate(const Board<T>& board) const override;

private:
    /**
     * @brief Oblicza docelową pozycję kafelka o danej wartości
     * @param value Wartość kafelka
     * @param size Rozmiar planszy
     * @return Para współrzędnych (x, y) docelowej pozycji
     */
    [[nodiscard]] std::pair<int, int> getTargetPosition(const T& value, int size) const;
};

#include "ManhattanDistance.tpp"

#endif
