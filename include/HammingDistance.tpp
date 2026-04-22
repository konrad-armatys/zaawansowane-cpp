#ifndef HAMMINGDISTANCE_TPP
#define HAMMINGDISTANCE_TPP

#include "HammingDistance.h"

template<PuzzleTile T>
double HammingDistance<T>::calculate(const Board<T>& board) const {
    const int size = board.getSize();
    int misplacedTiles = 0;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const T value = board.at(x, y);

            const T targetValue = getTargetValue(x, y, size);
            if (value != targetValue) {
                ++misplacedTiles;
            }
        }
    }

    return static_cast<double>(misplacedTiles);
}

template<PuzzleTile T>
T HammingDistance<T>::getTargetValue(int x, int y, int size) const {
    const int index = y * size + x;

    if (index == size * size - 1) {
        return T{};
    }

    return T(index + 1);
}

#endif // HAMMINGDISTANCE_TPP
