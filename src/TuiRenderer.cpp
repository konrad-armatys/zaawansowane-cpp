#include "TuiRenderer.h"
#include "PuzzlePresenter.h"
#include "ManhattanDistance.h"
#include "ui/TileRenderer.h"
#include "ui/GridRenderer.h"
#include "ui/StatsPanelRenderer.h"
#include "ui/MessagePanelRenderer.h"
#include "ui/ControlsPanelRenderer.h"
#include "ui/GameLayoutComposer.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <sstream>
#include <iomanip>

using namespace ftxui;

TuiRenderer::TuiRenderer(int boardSize, std::unique_ptr<IHeuristic<int>> heuristic)
    : lastMessage_(""),
      lastMovesCount_(0),
      lastUndoCount_(0),
      lastCorrectTiles_(0),
      lastHeuristicValue_(0.0),
      boardSize_(boardSize),
      heuristic_(std::move(heuristic)),
      hintPosition_(std::nullopt) {
    tileRenderer_ = std::make_shared<TileRenderer>();
    auto gridRenderer = std::make_shared<GridRenderer>(tileRenderer_);
    auto statsRenderer = std::make_shared<StatsPanelRenderer>();
    auto messageRenderer = std::make_shared<MessagePanelRenderer>();
    auto controlsRenderer = std::make_shared<ControlsPanelRenderer>();

    layoutComposer_ = std::make_shared<GameLayoutComposer>(
        gridRenderer, statsRenderer, messageRenderer, controlsRenderer
    );
}

void TuiRenderer::setHeuristic(std::unique_ptr<IHeuristic<int>> heuristic) {
    heuristic_ = std::move(heuristic);
}

bool TuiRenderer::isTileCorrect(int value, int x, int y, int boardSize) const {
    if (value == 0) {
        return (y == boardSize - 1 && x == boardSize - 1);
    }
    int expectedValue = y * boardSize + x + 1;
    return value == expectedValue;
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

Element TuiRenderer::createControlsPanel() const {
    return text("Sterowanie: ↑↓←→/WSAD - ruch | U - undo | R - redo | H - hint | V - save | L - load | N - new |  Q - quit") |
           dim | center;
}

Element TuiRenderer::createGameElement(const PuzzleViewState& state) {
    lastMovesCount_ = state.stats.movesCount.get();
    lastUndoCount_ = state.stats.undoCount.get();
    lastCorrectTiles_ = state.stats.correctTiles.get();
    lastHeuristicValue_ = state.heuristicValue;
    lastMessage_ = state.statusMessage;
    hintPosition_ = state.hintPosition;

    tileRenderer_->setHintPosition(state.hintPosition);

    return layoutComposer_->compose(state);
}

Element TuiRenderer::createGameElement(const PuzzlePresenter& presenter) {
    return createGameElement(presenter.getViewState());
}

void TuiRenderer::render(const Board<int>& board) {
    if (heuristic_) {
        lastHeuristicValue_ = heuristic_->calculate(board);
    }



    tileRenderer_->setHintPosition(hintPosition_);
    auto gridRenderer = std::make_shared<GridRenderer>(tileRenderer_);
    auto gridElement = gridRenderer->render(board);
    auto statsElement = createStatsPanel();
    auto messageElement = createMessagePanel();

    Elements mainContent;

    mainContent.push_back(
        text("UKŁADANKI Z LAT DZIECINNYCH") |
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
    mainContent.push_back(createControlsPanel());

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
    if (tileRenderer_) {
        tileRenderer_->setHintPosition(position);
    }
}
