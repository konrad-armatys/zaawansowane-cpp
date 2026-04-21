#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Board.h"
#include "GameStats.h"
#include "UndoRedoManager.h"
#include <stack>
#include <vector>

/**
 * @brief Struktura agregująca kompletny stan gry
 *
 * Zawiera wszystkie informacje potrzebne do zapisu i odczytu stanu gry,
 * włączając w to planszę, statystyki oraz pełną historię ruchów (stosy A i B).
 */
struct GameState {
    Board<int> board;
    GameStats stats;
    std::vector<MoveData> undoHistory;
    std::vector<MoveData> redoHistory;

    /**
     * @brief Konstruktor tworzący stan gry z istniejących komponentów
     * @param b Plansza gry
     * @param s Statystyki gry
     * @param undoHist Historia ruchów do cofnięcia
     * @param redoHist Historia ruchów do ponowienia
     */
    GameState(Board<int> b, GameStats s,
              std::vector<MoveData> undoHist = {},
              std::vector<MoveData> redoHist = {})
        : board(std::move(b)),
          stats(std::move(s)),
          undoHistory(std::move(undoHist)),
          redoHistory(std::move(redoHist)) {}

    /**
     * @brief Konstruktor domyślny
     * @param boardSize Rozmiar planszy (domyślnie 3x3)
     */
    explicit GameState(int boardSize = 3)
        : board(boardSize),
          stats(boardSize),
          undoHistory(),
          redoHistory() {}
};

#endif
