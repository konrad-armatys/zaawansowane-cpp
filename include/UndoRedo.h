#ifndef UNDOREDO_H
#define UNDOREDO_H

#include "Board.h"
#include <stack>
#include <memory>

/**
 * @brief Struktura reprezentująca stan gry
 */
struct GameState {
    Board<int> board;
    int emptyX;
    int emptyY;
    int moveCount;

    /**
     * @brief Konstruktor stanu gry
     * @param b Plansza
     * @param x Współrzędna x pustego pola
     * @param y Współrzędna y pustego pola
     * @param moves Liczba wykonanych ruchów
     */
    GameState(Board<int> b, int x, int y, int moves)
        : board(std::move(b)), emptyX(x), emptyY(y), moveCount(moves) {}
};

/**
 * @brief Klasa implementująca mechanizm Undo/Redo (Stosy A i B)
 *
 * Stos A przechowuje historię stanów do cofania (Undo)
 * Stos B przechowuje cofnięte stany do ponownego wykonania (Redo)
 */
class UndoRedo {
private:
    std::stack<GameState> undoStack_;
    std::stack<GameState> redoStack_;
    size_t maxHistorySize_;

    /**
     * @brief Ogranicza rozmiar stosu do maksymalnej wartości
     * @param stack Stos do ograniczenia
     */
    void limitStackSize(std::stack<GameState>& stack);

public:
    /**
     * @brief Konstruktor z opcjonalnym limitem historii
     * @param maxHistorySize Maksymalny rozmiar historii (0 = nielimitowany)
     */
    explicit UndoRedo(size_t maxHistorySize = 100);

    /**
     * @brief Zapisuje nowy stan gry do historii
     * @param board Aktualna plansza
     * @param emptyX Współrzędna x pustego pola
     * @param emptyY Współrzędna y pustego pola
     * @param moveCount Liczba wykonanych ruchów
     */
    void pushState(const Board<int>& board, int emptyX, int emptyY, int moveCount);

    /**
     * @brief Cofa ostatni ruch (Undo)
     * @return Stan gry przed ostatnim ruchem lub std::nullopt jeśli brak historii
     */
    [[nodiscard]] std::optional<GameState> undo();

    /**
     * @brief Przywraca cofnięty ruch (Redo)
     * @return Przywrócony stan gry lub std::nullopt jeśli brak cofniętych ruchów
     */
    [[nodiscard]] std::optional<GameState> redo();

    /**
     * @brief Sprawdza czy możliwe jest cofnięcie ruchu
     * @return true jeśli możliwe jest Undo
     */
    [[nodiscard]] bool canUndo() const noexcept;

    /**
     * @brief Sprawdza czy możliwe jest przywrócenie ruchu
     * @return true jeśli możliwe jest Redo
     */
    [[nodiscard]] bool canRedo() const noexcept;

    /**
     * @brief Czyści całą historię Undo i Redo
     */
    void clear();

    /**
     * @brief Czyści stos Redo (wywoływane przy nowym ruchu po Undo)
     */
    void clearRedoStack();

    /**
     * @brief Zwraca rozmiar stosu Undo
     * @return Liczba stanów w historii Undo
     */
    [[nodiscard]] size_t getUndoStackSize() const noexcept;

    /**
     * @brief Zwraca rozmiar stosu Redo
     * @return Liczba stanów w historii Redo
     */
    [[nodiscard]] size_t getRedoStackSize() const noexcept;

    /**
     * @brief Ustawia maksymalny rozmiar historii
     * @param maxSize Nowy maksymalny rozmiar (0 = nielimitowany)
     */
    void setMaxHistorySize(size_t maxSize);
};

#endif
