#include "ManhattanDistance.h"
#include <cmath>

double ManhattanDistance::calculate(const Board<int>& board) const {
    const int size = board.getSize();
    double totalDistance = 0.0;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int value = board.at(x, y);


            if (value == 0) {
                continue;
            }


            const auto [targetX, targetY] = getTargetPosition(value, size);


            totalDistance += std::abs(x - targetX) + std::abs(y - targetY);
        }
    }

    return totalDistance;
}

std::pair<int, int> ManhattanDistance::getTargetPosition(int value, int size) const {

    if (value == 0) {
        return {size - 1, size - 1};
    }



    const int targetIndex = value - 1;
    const int targetX = targetIndex % size;
    const int targetY = targetIndex / size;

    return {targetX, targetY};
}
