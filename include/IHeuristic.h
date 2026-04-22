#ifndef IHEURISTIC_H
#define IHEURISTIC_H

#include "Board.h"
#include "PuzzleConcepts.h"

/**
 * @brief Abstrakcyjna klasa bazowa dla heurystyk oceny planszy
 * @tparam T Typ elementu planszy, musi spełniać PuzzleTile concept.
 *           Typ musi być kopiowany, porównywalny i domyślnie inicjalizowalny.
 */
template<PuzzleTile T>
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
    [[nodiscard]] virtual double calculate(const Board<T>& board) const = 0;
};

#endif
