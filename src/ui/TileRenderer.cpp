#include "ui/TileRenderer.h"
#include <sstream>
#include <iomanip>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int TileRenderer::calculateTileWidth(int boardSize) const {

    if (boardSize <= 5) return 5;
    if (boardSize <= 10) return 4;
    if (boardSize <= 20) return 3;
    return 2;
}


bool TileRenderer::isCorrect(int value, int x, int y, int boardSize) const {

    if (value == 0) return false;


    int correctX = (value - 1) % boardSize;
    int correctY = (value - 1) / boardSize;

    return x == correctX && y == correctY;
}

void TileRenderer::setHintPosition(std::optional<std::pair<int, int>> position) {
    hintPosition_ = position;
}

std::optional<std::pair<int, int>> TileRenderer::getHintPosition() const noexcept {
    return hintPosition_;
}

Element TileRenderer::render(int value, int x, int y, int boardSize) const {
    int width = calculateTileWidth(boardSize);

    std::ostringstream oss;
    if (value == 0) {
        oss << std::setw(width) << " ";
        return text(oss.str()) | border | bgcolor(Color::Black);
    }

    oss << std::setw(width) << value;
    Element tile = text(oss.str()) | border;

    if (hintPosition_ && hintPosition_->first == x && hintPosition_->second == y) {
        tile = tile | bgcolor(Color::Yellow) | color(Color::Black) | bold;
    } else if (isCorrect(value, x, y, boardSize)) {
        tile = tile | bgcolor(Color::Green) | color(Color::Black);
    } else {
        tile = tile | bgcolor(Color::Blue) | color(Color::White);
    }

    return tile;
}