#include "TuiRenderer.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <sstream>
#include <iomanip>

using namespace ftxui;

TuiRenderer::TuiRenderer(int boardSize, std::unique_ptr<IHeuristic> heuristic)
    : lastMessage_(""),
      lastMovesCount_(0),
      lastUndoCount_(0),
      lastCorrectTiles_(0),
      lastHeuristicValue_(0.0),
      boardSize_(boardSize),
      heuristic_(std::move(heuristic)),
      hintPosition_(std::nullopt) {}

void TuiRenderer::setHeuristic(std::unique_ptr<IHeuristic> heuristic) {
    heuristic_ = std::move(heuristic);
}

bool TuiRenderer::isTileCorrect(int value, int x, int y, int boardSize) const {
    if (value == 0) {
        return (y == boardSize - 1 && x == boardSize - 1);
    }
    int expectedValue = y * boardSize + x + 1;
    return value == expectedValue;
}

Element TuiRenderer::createTile(int value, int x, int y, int boardSize) const {
    if (value == 0) {
        return text("   ") | border | bgcolor(Color::Black);
    }

    std::ostringstream oss;
    int width = 3;
    if (boardSize > 10) width = 4;
    if (boardSize > 31) width = 5;

    oss << std::setw(width) << value;

    Element tileElement = text(oss.str()) | border;


    if (hintPosition_.has_value() && hintPosition_->first == x && hintPosition_->second == y) {
        tileElement = tileElement | bgcolor(Color::Yellow) | color(Color::Black) | bold;
    }
    else if (isTileCorrect(value, x, y, boardSize)) {
        tileElement = tileElement | bgcolor(Color::Green) | color(Color::Black);
    } else {
        tileElement = tileElement | bgcolor(Color::Blue) | color(Color::White);
    }

    return tileElement;
}

Element TuiRenderer::createGrid(const Board<int>& board) const {
    const int size = board.getSize();

    if (size > 20) {
        Elements rows;
        int displayStart = 0;
        int displayEnd = std::min(size, 20);

        rows.push_back(text("Showing rows " + std::to_string(displayStart) + "-" +
                           std::to_string(displayEnd - 1) + " of " + std::to_string(size)) |
                      bold | color(Color::Yellow));

        for (int y = displayStart; y < displayEnd; ++y) {
            Elements row;
            for (int x = 0; x < std::min(size, 20); ++x) {
                row.push_back(createTile(board.at(x, y), x, y, size));
            }
            if (size > 20) {
                row.push_back(text("...") | dim);
            }
            rows.push_back(hbox(std::move(row)));
        }

        if (size > 20) {
            rows.push_back(text("...") | dim);
        }

        return vbox(std::move(rows)) | border | borderStyled(ROUNDED);
    } else {
        Elements rows;
        for (int y = 0; y < size; ++y) {
            Elements row;
            for (int x = 0; x < size; ++x) {
                row.push_back(createTile(board.at(x, y), x, y, size));
            }
            rows.push_back(hbox(std::move(row)));
        }
        return vbox(std::move(rows)) | border | borderStyled(ROUNDED);
    }
}

Element TuiRenderer::createStatsPanel() const {
    Elements stats;

    stats.push_back(text("=== STATYSTYKI ===") | bold | color(Color::Cyan));
    stats.push_back(separator());

    stats.push_back(hbox({
        text("Ruchy: ") | bold,
        text(std::to_string(lastMovesCount_)) | color(Color::Yellow)
    }));

    stats.push_back(hbox({
        text("Cofnięcia: ") | bold,
        text(std::to_string(lastUndoCount_)) | color(Color::Yellow)
    }));

    stats.push_back(hbox({
        text("Poprawne kafelki: ") | bold,
        text(std::to_string(lastCorrectTiles_) + " / " +
             std::to_string(boardSize_ * boardSize_)) | color(Color::Green)
    }));

    if (heuristic_) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << lastHeuristicValue_;
        stats.push_back(hbox({
            text("Heurystyka: ") | bold,
            text(oss.str()) | color(Color::Magenta)
        }));
    }

    return vbox(std::move(stats)) | border | borderStyled(ROUNDED) | size(WIDTH, GREATER_THAN, 30);
}

Element TuiRenderer::createMessagePanel() const {
    if (lastMessage_.empty()) {
        return text("");
    }

    return text(lastMessage_) |
           border |
           borderStyled(ROUNDED) |
           bgcolor(Color::Blue) |
           color(Color::White) |
           bold;
}

void TuiRenderer::render(const Board<int>& board) {
    if (heuristic_) {
        lastHeuristicValue_ = heuristic_->calculate(board);
    }

    auto gridElement = createGrid(board);
    auto statsElement = createStatsPanel();
    auto messageElement = createMessagePanel();

    Elements mainContent;

    mainContent.push_back(
        text("N-PUZZLE GAME") |
        bold |
        color(Color::Cyan) |
        center
    );

    mainContent.push_back(separator());

    Elements gameArea;
    gameArea.push_back(gridElement | flex);
    gameArea.push_back(separator());
    gameArea.push_back(statsElement);

    mainContent.push_back(hbox(std::move(gameArea)));

    if (!lastMessage_.empty()) {
        mainContent.push_back(separator());
        mainContent.push_back(messageElement);
    }

    mainContent.push_back(separator());
    mainContent.push_back(
        text("Sterowanie: ↑↓←→/WSAD - ruch | U - undo | R - redo | H - hint | V - save | L - load | N - new | T - reset | Q - quit") |
        dim | center
    );

    auto document = vbox(std::move(mainContent));

    auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
    Render(screen, document);
    screen.Print();
}

void TuiRenderer::updateStats(const GameStats& stats, double heuristicValue) {
    lastMovesCount_ = stats.movesCount.get();
    lastUndoCount_ = stats.undoCount.get();
    lastCorrectTiles_ = stats.correctTiles.get();
    lastHeuristicValue_ = heuristicValue;
}

void TuiRenderer::showMessage(const std::string& message) {
    lastMessage_ = message;
}

void TuiRenderer::refresh() {
}

void TuiRenderer::setHintHighlight(std::optional<std::pair<int, int>> position) {
    hintPosition_ = position;
}
