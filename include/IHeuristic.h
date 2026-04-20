#ifndef IHEURISTIC_H
#define IHEURISTIC_H

#include "Board.h"

/**
 * @brief Abstrakcyjna klasa bazowa dla heurystyk oceny planszy
 */
class IHeuristic {
public:
    /**
     * @brief Wirtualny destruktor
     */
    virtual ~IHeuristic() = default;

    /**
     * @brief Oblicza heurystyczną ocenę planszy
     * @param board Plansza do oceny
     * @return Wartość heurystyki (im mniejsza, tym bliżej rozwiązania)
     */
    [[nodiscard]] virtual double calculate(const Board<int>& board) const = 0;
};

#endif
