#include "ui/GridRenderer.h"
#include <algorithm>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

GridRenderer::GridRenderer(std::shared_ptr<TileRenderer> tileRenderer)
    : tileRenderer_(std::move(tileRenderer)) {}

ftxui::Element GridRenderer::createStandardGrid(const Board<int>& board) const {
    const int size = board.getSize();
    Elements rows;
    for (int y = 0; y < size; ++y) {
        Elements row;
        for (int x = 0; x < size; ++x) {
            row.push_back(tileRenderer_->render(board.at(x, y), x, y, size));
        }
        rows.push_back(hbox(std::move(row)));
    }
    return vbox(std::move(rows)) | borderStyled(ROUNDED);
}

ftxui::Element GridRenderer::createLargeGridView(const Board<int>& board, int maxVisible) const {
    const int size = board.getSize();
    const int displayEnd = std::min(size, maxVisible);
    Elements rows;

    rows.push_back(
        text("Wyświetlam wiersze 0-" + std::to_string(displayEnd - 1) + " z " + std::to_string(size)) |
        bold | color(Color::Yellow)
    );

    for (int y = 0; y < displayEnd; ++y) {
        Elements row;
        for (int x = 0; x < std::min(size, maxVisible); ++x) {
            row.push_back(tileRenderer_->render(board.at(x, y), x, y, size));
        }
        row.push_back(text("...") | dim);
        rows.push_back(hbox(std::move(row)));
    }
    rows.push_back(text("...") | dim);

    return vbox(std::move(rows)) | borderStyled(ROUNDED);
}

ftxui::Element GridRenderer::render(const Board<int>& board) const {
    const int size = board.getSize();
    if (size > 20) {
        return createLargeGridView(board, 20);
    }
    return createStandardGrid(board);
}
