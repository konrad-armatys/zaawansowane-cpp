#ifndef TEXTGAMESAVER_H
#define TEXTGAMESAVER_H

#include "IGameSaver.h"
#include <sstream>

/**
 * @brief Implementacja zapisu/odczytu stanu gry w formacie tekstowym (.txt)
 *
 * Format pliku (4-liniowy):
 * Linia 1: Rozmiar planszy (N)
 * Linia 2: Kafelki planszy (N*N liczb oddzielonych spacjami, 0 = puste pole)
 * Linia 3: Statystyki (movesCount undoCount correctTiles)
 * Linia 4: Historia ruchów w formacie:
 *          undoStackSize redoStackSize [fromX fromY toX toY] [fromX fromY toX toY] ...
 *          Pierwsze undoStackSize ruchów to stos Undo, reszta to stos Redo
 *
 * Przykład dla planszy 3x3:
 * 3
 * 1 2 3 4 5 6 7 8 0
 * 5 2 9
 * 2 1 0 1 1 2 1 2 2 1
 */
class TextGameSaver : public IGameSaver {
private:
    /**
     * @brief Parsuje pojedynczy ruch z strumienia
     * @param ss Strumień wejściowy
     * @return Sparsowany ruch
     * @throws std::invalid_argument Gdy nie udało się sparsować ruchu
     */
    MoveData parseMove(std::istringstream& ss) const;

    /**
     * @brief Waliduje rozmiar planszy
     * @param size Rozmiar planszy
     * @throws std::invalid_argument Gdy rozmiar jest nieprawidłowy
     */
    void validateBoardSize(int size) const;

    /**
     * @brief Parsuje linię z liczbami całkowitymi
     * @param line Linia do sparsowania
     * @return Wektor sparsowanych liczb
     * @throws std::invalid_argument Gdy parsowanie się nie powiodło
     */
    std::vector<int> parseIntegerLine(const std::string& line) const;

public:
    /**
     * @brief Konstruktor domyślny
     */
    TextGameSaver() = default;

    /**
     * @brief Zapisuje stan gry do pliku tekstowego
     *
     * Dla planszy większych niż 100x100 optymalizuje wydajność
     * poprzez wyłączenie synchronizacji stdio.
     *
     * @param path Ścieżka do pliku docelowego
     * @param state Stan gry do zapisania
     * @throws std::runtime_error Gdy nie można otworzyć pliku lub zapis się nie powiedzie
     */
    void save(const std::string& path, const GameState& state) override;

    /**
     * @brief Wczytuje stan gry z pliku tekstowego
     * @param path Ścieżka do pliku źródłowego
     * @return Wczytany stan gry
     * @throws std::runtime_error Gdy plik nie istnieje lub nie można go otworzyć
     * @throws std::invalid_argument Gdy plik ma nieprawidłowy format lub jest uszkodzony
     */
    GameState load(const std::string& path) override;
};

#endif
