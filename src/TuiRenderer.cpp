#include "TuiRenderer.h"
#include "PuzzlePresenter.h"
#include "ui/GameLayoutComposer.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace ftxui;

TuiRenderer::TuiRenderer(int boardSize, std::unique_ptr<IHeuristic<int>> heuristic)
    : lastMessage_(""),
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

Element TuiRenderer::createGameElement(const PuzzleViewState& state) {
    hintPosition_ = state.hintPosition;
    tileRenderer_->setHintPosition(state.hintPosition);
    return layoutComposer_->compose(state);
}

Element TuiRenderer::createGameElement(const PuzzlePresenter& presenter) {
    return createGameElement(presenter.getViewState());
}

void TuiRenderer::render(const Board<int>& board) {
    double heuristicValue = heuristic_ ? heuristic_->calculate(board) : 0.0;

    GameStats tempStats(boardSize_);
    tempStats.update(board);

    PuzzleViewState state{board, tempStats, lastMessage_,
                          InputMode::None, "", heuristicValue, hintPosition_};
    auto document = createGameElement(state);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
    Render(screen, document);
    screen.Print();
}

void TuiRenderer::updateStats(const GameStats& /*stats*/, double /*heuristicValue*/) {
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
