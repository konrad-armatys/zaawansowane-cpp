#ifndef TUIRENDERER_H
#define TUIRENDERER_H

#include "IPuzzleView.h"
#include "IHeuristic.h"
#include "Board.h"
#include "GameStats.h"
#include "PuzzlePresenter.h"
#include "ui/GameLayoutComposer.h"
#include "ui/TileRenderer.h"
#include <memory>
#include <string>
#include <optional>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

/**
 * @brief Implementacja TUI (Text User Interface) dla gry N-Puzzle używająca FTXUI
 *
 * Fasada delegująca renderowanie do wyspecjalizowanych komponentów UI.
 * Klasa renderuje planszę gry w terminalu z kolorową stylizacją,
 * panelem statystyk oraz obsługą dużych plansz (do 101x101).
 */
class TuiRenderer : public IPuzzleView {
private:
    std::string lastMessage_;
    int lastMovesCount_;
    int lastUndoCount_;
    int lastCorrectTiles_;
    double lastHeuristicValue_;
    int boardSize_;
    std::unique_ptr<IHeuristic<int>> heuristic_;
    std::optional<std::pair<int, int>> hintPosition_;
    std::shared_ptr<GameLayoutComposer> layoutComposer_;
    std::shared_ptr<TileRenderer> tileRenderer_;

    /**
     * @brief Tworzy element DOM panelu statystyk (używa ostatnio zapamiętanych wartości)
     * @return Element DOM reprezentujący panel statystyk
     */
    ftxui::Element createStatsPanel() const;

    /**
     * @brief Tworzy element DOM dla wiadomości (używa ostatnio zapamiętanej wartości)
     * @return Element DOM reprezentujący wiadomość
     */
    ftxui::Element createMessagePanel() const;

    /**
     * @brief Tworzy element DOM z tekstem pomocy sterowania
     * @return Element DOM reprezentujący pasek pomocy
     */
    ftxui::Element createControlsPanel() const;

public:
    /**
     * @brief Sprawdza czy kafelek jest na właściwym miejscu
     * @param value Wartość kafelka
     * @param x Współrzędna x
     * @param y Współrzędna y
     * @param boardSize Rozmiar planszy
     * @return true jeśli kafelek jest na właściwym miejscu
     */
    bool isTileCorrect(int value, int x, int y, int boardSize) const;
    /**
     * @brief Konstruktor tworzący renderer TUI
     * @param boardSize Rozmiar planszy (N x N)
     * @param heuristic Wskaźnik do implementacji heurystyki<int> (opcjonalny)
     */
    explicit TuiRenderer(int boardSize, std::unique_ptr<IHeuristic<int>> heuristic = nullptr);

    /**
     * @brief Ustawia heurystykę do obliczania oceny planszy
     * @param heuristic Wskaźnik do implementacji heurystyki<int>
     */
    void setHeuristic(std::unique_ptr<IHeuristic<int>> heuristic);

    void render(const Board<int>& board) override;
    void updateStats(const GameStats& stats, double heuristicValue = 0.0) override;
    void showMessage(const std::string& message) override;
    void refresh() override;

    /**
     * @brief Ustawia pozycję kafelka do podświetlenia jako podpowiedź
     * @param position Opcjonalna pozycja (x, y) kafelka (nullopt aby wyłączyć)
     */
    void setHintHighlight(std::optional<std::pair<int, int>> position);

    /**
     * @brief Zwraca pozycję podświetlonego kafelka (podpowiedź)
     * @return Opcjonalna pozycja (x, y) lub nullopt jeśli brak podświetlenia
     */
    std::optional<std::pair<int, int>> getHintPosition() const { return hintPosition_; }

    /**
     * @brief Buduje kompletny element UI gry na podstawie stanu widoku
     * @param state Zagregowany stan widoku dostarczony przez prezenter
     * @return Element FTXUI gotowy do renderowania w głównej pętli
     */
    ftxui::Element createGameElement(const PuzzleViewState& state);

    /**
     * @brief Buduje kompletny element UI gry, pobierając stan z prezentera
     * @param presenter Prezenter dostarczający dane do renderowania przez getViewState()
     * @return Element FTXUI gotowy do renderowania w głównej pętli
     */
    ftxui::Element createGameElement(const PuzzlePresenter& presenter);
};

#endif
