#include "HammingDistance.h"

double HammingDistance::calculate(const Board<int>& board) const {
    const int size = board.getSize();
    int misplacedTiles = 0;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int value = board.at(x, y);


            if (value == 0) {
                continue;
            }


            const int targetValue = getTargetValue(x, y, size);
            if (value != targetValue) {
                ++misplacedTiles;
            }
        }
    }

    return static_cast<double>(misplacedTiles);
}

int HammingDistance::getTargetValue(int x, int y, int size) const {


    const int index = y * size + x;

    if (index == size * size - 1) {
        return 0;
    }

    return index + 1;
}
