#include "ui/StatsPanelRenderer.h"
#include <sstream>
#include <iomanip>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Element StatsPanelRenderer::createStatRow(const std::string& label,
                                                   const std::string& value,
                                                   ftxui::Color color) const {
    return hbox({
        text(label) | bold,
        text(value) | ftxui::color(color)
    });
}

ftxui::Element StatsPanelRenderer::render(const GameStats& stats,
                                           double heuristicValue,
                                           int boardSize) const {
    Elements statsElements;

    statsElements.push_back(text("=== STATYSTYKI ===") | bold | color(Color::Cyan));
    statsElements.push_back(separator());

    statsElements.push_back(createStatRow(
        "Ruchy: ",
        std::to_string(stats.movesCount.get()),
        Color::Yellow
    ));

    statsElements.push_back(createStatRow(
        "Cofnięcia: ",
        std::to_string(stats.undoCount.get()),
        Color::Yellow
    ));

    statsElements.push_back(createStatRow(
        "Poprawne kafelki: ",
        std::to_string(stats.correctTiles.get()) + " / " +
            std::to_string(boardSize * boardSize),
        Color::Green
    ));

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << heuristicValue;
    statsElements.push_back(createStatRow(
        "Heurystyka: ",
        oss.str(),
        Color::Magenta
    ));

    return vbox(std::move(statsElements)) | size(WIDTH, GREATER_THAN, 30);
}
