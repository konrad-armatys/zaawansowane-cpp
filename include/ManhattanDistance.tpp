#ifndef MANHATTANDISTANCE_TPP
#define MANHATTANDISTANCE_TPP

#include "ManhattanDistance.h"
#include <cmath>

template<NumericTile T>
double ManhattanDistance<T>::calculate(const Board<T>& board) const {
    const int size = board.getSize();
    double totalDistance = 0.0;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const T value = board.at(x, y);

            if (static_cast<int>(value) == 0) {
                continue;
            }

            const auto [targetX, targetY] = getTargetPosition(value, size);

            totalDistance += std::abs(x - targetX) + std::abs(y - targetY);
        }
    }

    return totalDistance;
}

template<NumericTile T>
std::pair<int, int> ManhattanDistance<T>::getTargetPosition(const T& value, int size) const {
    const int intValue = static_cast<int>(value);

    if (intValue == 0) {
        return {size - 1, size - 1};
    }

    const int targetIndex = intValue - 1;
    const int targetX = targetIndex % size;
    const int targetY = targetIndex / size;

    return {targetX, targetY};
}

#endif // MANHATTANDISTANCE_TPP
