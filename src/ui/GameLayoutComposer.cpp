#include "ui/GameLayoutComposer.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

GameLayoutComposer::GameLayoutComposer(std::shared_ptr<GridRenderer> gridRenderer,
                                        std::shared_ptr<StatsPanelRenderer> statsRenderer,
                                        std::shared_ptr<MessagePanelRenderer> messageRenderer,
                                        std::shared_ptr<ControlsPanelRenderer> controlsRenderer)
    : gridRenderer_(std::move(gridRenderer)),
      statsRenderer_(std::move(statsRenderer)),
      messageRenderer_(std::move(messageRenderer)),
      controlsRenderer_(std::move(controlsRenderer)) {}

ftxui::Element GameLayoutComposer::createTitleBanner() const {
    return text("UKŁADANKI Z LAT DZIECINNYCH") | bold | color(Color::Cyan) | center;
}

ftxui::Element GameLayoutComposer::createMainLayout(const PuzzleViewState& state) {
    const int boardSize = state.board.getSize();

    Element gridElement = gridRenderer_->render(state.board);
    Element statsElement = statsRenderer_->render(state.stats, state.heuristicValue, boardSize);

    Elements gameArea;
    gameArea.push_back(gridElement | flex);
    gameArea.push_back(separator());
    gameArea.push_back(statsElement);

    return hbox(std::move(gameArea));
}

ftxui::Element GameLayoutComposer::compose(const PuzzleViewState& state) {
    Elements mainContent;

    mainContent.push_back(createTitleBanner());
    mainContent.push_back(separator());
    mainContent.push_back(createMainLayout(state));

    if (!state.statusMessage.empty()) {
        mainContent.push_back(messageRenderer_->renderMessage(state.statusMessage));
    }

    if (state.inputMode != InputMode::None) {
        mainContent.push_back(separator());
        mainContent.push_back(messageRenderer_->renderInput(state.inputMode, state.inputBuffer));
    }

    mainContent.push_back(separator());
    mainContent.push_back(controlsRenderer_->render());

    return vbox(std::move(mainContent));
}
