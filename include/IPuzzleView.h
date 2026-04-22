#ifndef IPUZZLEVIEW_H
#define IPUZZLEVIEW_H

#include "Board.h"
#include "GameStats.h"

/**
 * @brief Abstrakcyjny interfejs widoku dla gry N-Puzzle
 */
class IPuzzleView {
public:
    /**
     * @brief Wirtualny destruktor
     */
    virtual ~IPuzzleView() = default;

    /**
     * @brief Renderuje aktualny stan planszy
     * @param board Plansza do wyświetlenia
     */
    virtual void render(const Board<int>& board) = 0;

    /**
     * @brief Aktualizuje wyświetlane statystyki
     * @param stats Statystyki gry
     * @param heuristicValue Opcjonalna wartość heurystyki
     */
    virtual void updateStats(const GameStats& stats, double heuristicValue = 0.0) = 0;

    /**
     * @brief Wyświetla wiadomość użytkownikowi
     * @param message Treść wiadomości
     */
    virtual void showMessage(const std::string& message) = 0;

    /**
     * @brief Odświeża widok
     */
    virtual void refresh() = 0;
};

#endif
