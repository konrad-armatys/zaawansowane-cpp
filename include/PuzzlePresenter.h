#ifndef PUZZLEPRESENTER_H
#define PUZZLEPRESENTER_H

#include "PuzzleEngine.h"
#include "UndoRedoManager.h"
#include "IGameSaver.h"
#include <memory>
#include <string>

/**
 * @brief Klasa prezentera zarządzająca interakcją między silnikiem gry a UI
 *
 * Implementuje wzorzec MVP (Model-View-Presenter), koordynując logikę gry,
 * historię ruchów oraz operacje zapisu/odczytu stanu gry.
 * Używa wzorca Strategy poprzez polimorficzny wskaźnik do IGameSaver.
 */
class PuzzlePresenter {
private:
    PuzzleEngine<int> engine_;
    UndoRedoManager undoRedoManager_;
    std::unique_ptr<IGameSaver> saveHandler_;

    /**
     * @brief Tworzy obiekt GameState z aktualnego stanu gry
     * @return Kompletny stan gry gotowy do zapisu
     */
    GameState captureCurrentState() const;

    /**
     * @brief Przywraca stan gry z obiektu GameState
     * @param state Stan gry do przywrócenia
     */
    void restoreState(const GameState& state);

public:
    /**
     * @brief Konstruktor tworzący prezenter z domyślną strategią zapisu
     * @param size Rozmiar planszy (N x N)
     * @param saver Wskaźnik do implementacji IGameSaver (domyślnie TextGameSaver)
     */
    explicit PuzzlePresenter(int size, std::unique_ptr<IGameSaver> saver = nullptr);

    /**
     * @brief Zapisuje aktualny stan gry do pliku
     * @param filename Ścieżka do pliku docelowego
     * @throws std::runtime_error Gdy nie ustawiono saveHandler lub zapis się nie powiedzie
     */
    void saveGame(const std::string& filename);

    /**
     * @brief Wczytuje stan gry z pliku
     * @param filename Ścieżka do pliku źródłowego
     * @throws std::runtime_error Gdy nie ustawiono saveHandler lub odczyt się nie powiedzie
     * @throws std::invalid_argument Gdy plik ma nieprawidłowy format
     */
    void loadGame(const std::string& filename);

    /**
     * @brief Zmienia strategię zapisu/odczytu
     * @param saver Nowy wskaźnik do implementacji IGameSaver
     */
    void setSaveHandler(std::unique_ptr<IGameSaver> saver);

    /**
     * @brief Pobiera referencję do silnika gry
     * @return Referencja do PuzzleEngine
     */
    PuzzleEngine<int>& getEngine() noexcept;

    /**
     * @brief Pobiera referencję do silnika gry (wersja const)
     * @return Stała referencja do PuzzleEngine
     */
    const PuzzleEngine<int>& getEngine() const noexcept;

    /**
     * @brief Pobiera referencję do managera undo/redo
     * @return Referencja do UndoRedoManager
     */
    UndoRedoManager& getUndoRedoManager() noexcept;

    /**
     * @brief Pobiera referencję do managera undo/redo (wersja const)
     * @return Stała referencja do UndoRedoManager
     */
    const UndoRedoManager& getUndoRedoManager() const noexcept;

    /**
     * @brief Wykonuje ruch w danym kierunku z obsługą historii
     * @param dir Kierunek ruchu
     * @return true jeśli ruch został wykonany, false w przeciwnym wypadku
     */
    bool move(Direction dir);

    /**
     * @brief Cofa ostatni ruch
     * @return true jeśli cofnięcie się powiodło, false w przeciwnym wypadku
     */
    bool undo();

    /**
     * @brief Ponawia cofnięty ruch
     * @return true jeśli ponowienie się powiodło, false w przeciwnym wypadku
     */
    bool redo();

    /**
     * @brief Tasuje planszę generując nową grę
     */
    void shuffle();

    /**
     * @brief Resetuje grę do stanu początkowego
     */
    void reset();
};

#endif
