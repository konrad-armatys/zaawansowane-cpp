#ifndef HAMMINGDISTANCE_H
#define HAMMINGDISTANCE_H

#include "IHeuristic.h"

/**
 * @brief Implementacja heurystyki odległości Hamminga
 *
 * Zlicza liczbę kafelków znajdujących się na niewłaściwych pozycjach.
 * Jest to prostsza i szybsza heurystyka niż Manhattan Distance,
 * ale może być mniej dokładna w ocenie odległości od rozwiązania.
 */
class HammingDistance : public IHeuristic {
public:
    /**
     * @brief Zlicza kafelki na niewłaściwych pozycjach
     * @param board Plansza do oceny
     * @return Liczba kafelków na błędnych pozycjach
     */
    [[nodiscard]] double calculate(const Board<int>& board) const override;

private:
    /**
     * @brief Oblicza docelową wartość dla danej pozycji
     * @param x Współrzędna x
     * @param y Współrzędna y
     * @param size Rozmiar planszy
     * @return Docelowa wartość kafelka na tej pozycji
     */
    [[nodiscard]] int getTargetValue(int x, int y, int size) const;
};

#endif
