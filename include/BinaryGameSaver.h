#ifndef BINARYGAMESAVER_H
#define BINARYGAMESAVER_H

#include "IGameSaver.h"

/**
 * @brief Implementacja zapisu/odczytu stanu gry w formacie binarnym
 *
 * Format pliku (little-endian):
 * 1. Magic number (4 bajty): "NPUZ" - identyfikator formatu
 * 2. Wersja formatu (4 bajty): uint32_t
 * 3. Rozmiar planszy (4 bajty): int32_t
 * 4. Dane planszy (size*size * 4 bajty): int32_t[]
 * 5. Statystyki (3 * 4 bajty): movesCount, undoCount, correctTiles (int32_t)
 * 6. Rozmiar stosu Undo (4 bajty): uint32_t
 * 7. Dane stosu Undo (undoSize * 16 bajtów): [fromX, fromY, toX, toY (int32_t)]
 * 8. Rozmiar stosu Redo (4 bajty): uint32_t
 * 9. Dane stosu Redo (redoSize * 16 bajtów): [fromX, fromY, toX, toY (int32_t)]
 *
 * Całkowity rozmiar: 16 + size*size*4 + 12 + 4 + undoSize*16 + 4 + redoSize*16 bajtów
 */
class BinaryGameSaver : public IGameSaver {
private:
    static constexpr uint32_t MAGIC_NUMBER = 0x5A55504E;
    static constexpr uint32_t FORMAT_VERSION = 1;

    /**
     * @brief Waliduje magic number i wersję formatu
     * @param magic Odczytany magic number
     * @param version Odczytana wersja
     * @throws std::invalid_argument Gdy format jest nieprawidłowy
     */
    void validateFormat(uint32_t magic, uint32_t version) const;

    /**
     * @brief Zapisuje pojedynczy ruch do strumienia
     * @param os Strumień wyjściowy
     * @param move Ruch do zapisania
     */
    void writeMove(std::ostream& os, const MoveData& move) const;

    /**
     * @brief Odczytuje pojedynczy ruch ze strumienia
     * @param is Strumień wejściowy
     * @return Odczytany ruch
     */
    MoveData readMove(std::istream& is) const;

public:
    /**
     * @brief Konstruktor domyślny
     */
    BinaryGameSaver() = default;

    /**
     * @brief Zapisuje stan gry do pliku binarnego
     * @param path Ścieżka do pliku docelowego
     * @param state Stan gry do zapisania
     * @throws std::runtime_error Gdy nie można otworzyć pliku lub zapis się nie powiedzie
     */
    void save(const std::string& path, const GameState& state) override;

    /**
     * @brief Wczytuje stan gry z pliku binarnego
     * @param path Ścieżka do pliku źródłowego
     * @return Wczytany stan gry
     * @throws std::runtime_error Gdy nie można otworzyć pliku lub odczyt się nie powiedzie
     * @throws std::invalid_argument Gdy plik ma nieprawidłowy format
     */
    GameState load(const std::string& path) override;
};

#endif
