#pragma once

#include "GameStats.h"
#include <string>
#include <ftxui/dom/elements.hpp>

/**
 * @brief Renders the game statistics panel
 */
class StatsPanelRenderer {
public:
    /**
     * @brief Konstruktor domyślny
     */
    StatsPanelRenderer() = default;

    /**
     * @brief Tworzy panel statystyk
     * @param stats Statystyki gry
     * @param heuristicValue Wartość heurystyki
     * @param boardSize Rozmiar planszy (do obliczenia łącznej liczby kafelków)
     * @return Element FTXUI
     */
    ftxui::Element render(const GameStats& stats,
                          double heuristicValue,
                          int boardSize) const;

private:
    /**
     * @brief Tworzy pojedynczy wiersz statystyki
     * @param label Etykieta (np. "Ruchy:")
     * @param value Wartość do wyświetlenia
     * @param color Kolor wartości
     * @return Element FTXUI reprezentujący wiersz
     */
    ftxui::Element createStatRow(const std::string& label,
                                  const std::string& value,
                                  ftxui::Color color) const;
};
