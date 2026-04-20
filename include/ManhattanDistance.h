#ifndef MANHATTANDISTANCE_H
#define MANHATTANDISTANCE_H

#include "IHeuristic.h"

/**
 * @brief Implementacja heurystyki odległości Manhattan
 *
 * Oblicza sumę odległości Manhattan wszystkich kafelków od ich docelowych pozycji.
 * Odległość Manhattan to suma bezwzględnych różnic współrzędnych.
 */
class ManhattanDistance : public IHeuristic {
public:
    /**
     * @brief Oblicza sumę odległości Manhattan dla wszystkich kafelków
     * @param board Plansza do oceny
     * @return Suma odległości Manhattan wszystkich kafelków
     */
    [[nodiscard]] double calculate(const Board<int>& board) const override;

private:
    /**
     * @brief Oblicza docelową pozycję kafelka o danej wartości
     * @param value Wartość kafelka
     * @param size Rozmiar planszy
     * @return Para współrzędnych (x, y) docelowej pozycji
     */
    [[nodiscard]] std::pair<int, int> getTargetPosition(int value, int size) const;
};

#endif
