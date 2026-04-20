#ifndef GAMESTATS_H
#define GAMESTATS_H

#include "Property.h"
#include "Board.h"
#include <functional>

/**
 * @brief Klasa śledząca statystyki gry N-Puzzle
 */
class GameStats {
private:
    int boardSize_;

public:
    Property<int> movesCount;
    Property<int> undoCount;
    Property<int> correctTiles;

    /**
     * @brief Konstruktor inicjalizujący statystyki dla danego rozmiaru planszy
     * @param boardSize Rozmiar planszy (N x N)
     */
    explicit GameStats(int boardSize = 3)
        : boardSize_(boardSize),
          movesCount(0),
          undoCount(0),
          correctTiles(0) {}

    /**
     * @brief Aktualizuje liczbę poprawnie ustawionych kafli
     * @param board Plansza do sprawdzenia
     */
    void update(const Board<int>& board) {
        int correct = 0;
        const int size = board.getSize();

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                int expectedValue = y * size + x + 1;
                if (y == size - 1 && x == size - 1) {
                    expectedValue = 0;
                }

                if (board.at(x, y) == expectedValue) {
                    ++correct;
                }
            }
        }

        correctTiles.set(correct);
    }

    /**
     * @brief Resetuje wszystkie statystyki do wartości początkowych
     */
    void reset() {
        movesCount.reset();
        undoCount.reset();
        correctTiles.reset();
    }

    /**
     * @brief Ustawia callback dla zmiany liczby ruchów
     * @param callback Funkcja wywoływana przy zmianie movesCount
     */
    void onMovesChange(std::function<void(int)> callback) {
        movesCount.setOnChange(std::move(callback));
    }

    /**
     * @brief Ustawia callback dla zmiany liczby cofnięć
     * @param callback Funkcja wywoływana przy zmianie undoCount
     */
    void onUndoChange(std::function<void(int)> callback) {
        undoCount.setOnChange(std::move(callback));
    }

    /**
     * @brief Ustawia callback dla zmiany liczby poprawnych kafli
     * @param callback Funkcja wywoływana przy zmianie correctTiles
     */
    void onCorrectTilesChange(std::function<void(int)> callback) {
        correctTiles.setOnChange(std::move(callback));
    }
};

#endif
