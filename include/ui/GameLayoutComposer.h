#pragma once

#include "PuzzlePresenter.h"
#include "ui/GridRenderer.h"
#include "ui/StatsPanelRenderer.h"
#include "ui/MessagePanelRenderer.h"
#include "ui/ControlsPanelRenderer.h"
#include <memory>
#include <ftxui/dom/elements.hpp>

/**
 * @brief Orchestrates all renderers into the final game layout
 */
class GameLayoutComposer {
public:
    /**
     * @brief Konstruktor z dependency injection wszystkich rendererów
     * @param gridRenderer Renderer siatki planszy
     * @param statsRenderer Renderer panelu statystyk
     * @param messageRenderer Renderer komunikatów i inputu
     * @param controlsRenderer Renderer paska pomocy
     */
    GameLayoutComposer(std::shared_ptr<GridRenderer> gridRenderer,
                       std::shared_ptr<StatsPanelRenderer> statsRenderer,
                       std::shared_ptr<MessagePanelRenderer> messageRenderer,
                       std::shared_ptr<ControlsPanelRenderer> controlsRenderer);

    /**
     * @brief Komponuje finalny element gry z wszystkich komponentów
     * @param state Stan widoku puzzli zawierający wszystkie dane
     * @return Element FTXUI gotowy do renderowania
     */
    ftxui::Element compose(const PuzzleViewState& state);

private:
    std::shared_ptr<GridRenderer> gridRenderer_;
    std::shared_ptr<StatsPanelRenderer> statsRenderer_;
    std::shared_ptr<MessagePanelRenderer> messageRenderer_;
    std::shared_ptr<ControlsPanelRenderer> controlsRenderer_;

    /**
     * @brief Tworzy baner tytułowy gry
     * @return Element FTXUI z tytułem
     */
    ftxui::Element createTitleBanner() const;

    /**
     * @brief Komponuje główny obszar gry (siatka + statystyki)
     * @param state Stan widoku puzzli
     * @return Element FTXUI z obszarem gry
     */
    ftxui::Element createMainLayout(const PuzzleViewState& state);
};
