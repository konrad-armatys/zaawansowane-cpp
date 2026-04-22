#ifndef TUIRENDERER_H
#define TUIRENDERER_H

#include "IPuzzleView.h"
#include "IHeuristic.h"
#include "Board.h"
#include "GameStats.h"
#include <memory>
#include <string>
#include <optional>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

class PuzzlePresenter;

/**
 * @brief Tryb wprowadzania danych przez użytkownika
 */
enum class ViewInputMode {
    None,
    Save,
    Load
};

/**
 * @brief Struktura agregująca stan potrzebny do wyrenderowania widoku
 *
 * Jest to kontrakt między prezenterem a widokiem: wszystkie dane niezbędne do
 * narysowania kompletnego ekranu gry są dostarczane w jednym obiekcie.
 */
struct ViewState {
    const Board<int>& board;
    const GameStats& stats;
    double heuristicValue;
    std::string statusMessage;
    ViewInputMode inputMode;
    std::string inputBuffer;
    std::optional<std::pair<int, int>> hintPosition;
};

/**
 * @brief Implementacja TUI (Text User Interface) dla gry N-Puzzle używająca FTXUI
 *
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
    std::unique_ptr<IHeuristic> heuristic_;
    std::optional<std::pair<int, int>> hintPosition_;

    /**
     * @brief Tworzy element DOM dla pojedynczego kafelka
     * @param value Wartość kafelka
     * @param x Współrzędna x
     * @param y Współrzędna y
     * @param boardSize Rozmiar planszy
     * @return Element DOM reprezentujący kafelek
     */
    ftxui::Element createTile(int value, int x, int y, int boardSize) const;

    /**
     * @brief Tworzy element DOM dla siatki planszy
     * @param board Plansza do wyrenderowania
     * @return Element DOM reprezentujący siatkę
     */
    ftxui::Element createGrid(const Board<int>& board) const;

    /**
     * @brief Tworzy element DOM panelu statystyk na podstawie dostarczonych danych
     * @param stats Statystyki gry
     * @param heuristicValue Wartość heurystyki
     * @param boardSize Rozmiar planszy (do obliczenia łącznej liczby pól)
     * @return Element DOM reprezentujący panel statystyk
     */
    ftxui::Element createStatsPanel(const GameStats& stats, double heuristicValue, int boardSize) const;

    /**
     * @brief Tworzy element DOM panelu statystyk (używa ostatnio zapamiętanych wartości)
     * @return Element DOM reprezentujący panel statystyk
     */
    ftxui::Element createStatsPanel() const;

    /**
     * @brief Tworzy element DOM dla wiadomości statusowej
     * @param message Treść wiadomości
     * @return Element DOM reprezentujący wiadomość
     */
    ftxui::Element createMessagePanel(const std::string& message) const;

    /**
     * @brief Tworzy element DOM dla wiadomości (używa ostatnio zapamiętanej wartości)
     * @return Element DOM reprezentujący wiadomość
     */
    ftxui::Element createMessagePanel() const;

    /**
     * @brief Tworzy element DOM pola wprowadzania ścieżki pliku (save/load)
     * @param mode Aktualny tryb wprowadzania
     * @param buffer Zawartość bufora wprowadzania
     * @return Element DOM reprezentujący pole wprowadzania (pusty, gdy tryb None)
     */
    ftxui::Element createInputPanel(ViewInputMode mode, const std::string& buffer) const;

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
     * @param heuristic Wskaźnik do implementacji heurystyki (opcjonalny)
     */
    explicit TuiRenderer(int boardSize, std::unique_ptr<IHeuristic> heuristic = nullptr);

    /**
     * @brief Ustawia heurystykę do obliczania oceny planszy
     * @param heuristic Wskaźnik do implementacji heurystyki
     */
    void setHeuristic(std::unique_ptr<IHeuristic> heuristic);

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
     * @param state Zagregowany stan widoku (plansza, statystyki, wiadomości, tryb wprowadzania)
     * @return Element FTXUI gotowy do renderowania w głównej pętli
     */
    ftxui::Element createGameElement(const ViewState& state);

    /**
     * @brief Buduje kompletny element UI gry na podstawie stanu prezentera
     * @param presenter Prezenter dostarczający dane do renderowania
     * @return Element FTXUI gotowy do renderowania w głównej pętli
     */
    ftxui::Element createGameElement(const PuzzlePresenter& presenter);
};

#endif
