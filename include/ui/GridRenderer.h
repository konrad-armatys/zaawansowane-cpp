#pragma once

#include "Board.h"
#include "ui/TileRenderer.h"
#include <memory>
#include <ftxui/dom/elements.hpp>

/**
 * @brief Renders the game board grid using TileRenderer
 */
class GridRenderer {
public:
    /**
     * @brief Konstruktor przyjmujący TileRenderer przez dependency injection
     * @param tileRenderer Wskaźnik do renderera kafelków
     */
    explicit GridRenderer(std::shared_ptr<TileRenderer> tileRenderer);

    /**
     * @brief Tworzy siatkę planszy
     * @param board Plansza do wyrenderowania
     * @return Element FTXUI z siatką w zaokrąglonym border
     */
    ftxui::Element render(const Board<int>& board) const;

private:
    std::shared_ptr<TileRenderer> tileRenderer_;

    /**
     * @brief Tworzy standardową siatkę dla plansz ≤20x20
     * @param board Plansza do wyrenderowania
     * @return Element FTXUI z pełną siatką
     */
    ftxui::Element createStandardGrid(const Board<int>& board) const;

    /**
     * @brief Tworzy widok siatki z obcięciem dla plansz >20x20
     * @param board Plansza do wyrenderowania
     * @param maxVisible Maksymalna liczba widocznych wierszy/kolumn
     * @return Element FTXUI z obciętą siatką i komunikatem
     */
    ftxui::Element createLargeGridView(const Board<int>& board, int maxVisible) const;
};
