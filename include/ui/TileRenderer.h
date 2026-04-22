#pragma once

#include <optional>
#include <utility>
#include <ftxui/dom/elements.hpp>

/**
 * @brief Renders individual tiles for the puzzle game board
 */
class TileRenderer {
public:
    /**
     * @brief Konstruktor domyślny
     */
    TileRenderer() = default;

    /**
     * @brief Tworzy element FTXUI reprezentujący kafelek
     * @param value Wartość kafelka (0 = puste pole)
     * @param x Pozycja X na planszy
     * @param y Pozycja Y na planszy
     * @param boardSize Rozmiar planszy
     * @return Element FTXUI
     */
    ftxui::Element render(int value, int x, int y, int boardSize) const;

    /**
     * @brief Sprawdza czy kafelek jest na właściwej pozycji
     * @param value Wartość kafelka
     * @param x Pozycja X na planszy
     * @param y Pozycja Y na planszy
     * @param boardSize Rozmiar planszy
     * @return true jeśli kafelek jest na właściwym miejscu
     */
    bool isCorrect(int value, int x, int y, int boardSize) const;

    /**
     * @brief Ustawia pozycję podpowiedzi (hint) do podświetlenia
     * @param position Opcjonalna pozycja (x, y) lub std::nullopt aby wyłączyć
     */
    void setHintPosition(std::optional<std::pair<int, int>> position);

    /**
     * @brief Pobiera aktualną pozycję podpowiedzi
     * @return Opcjonalna pozycja (x, y) lub std::nullopt
     */
    std::optional<std::pair<int, int>> getHintPosition() const noexcept;

private:
    std::optional<std::pair<int, int>> hintPosition_;

    /**
     * @brief Oblicza szerokość kafelka w znakach na podstawie rozmiaru planszy
     * @param boardSize Rozmiar planszy
     * @return Szerokość w znakach
     */
    int calculateTileWidth(int boardSize) const;

};