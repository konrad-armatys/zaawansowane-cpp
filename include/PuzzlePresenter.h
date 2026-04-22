#ifndef PUZZLEPRESENTER_H
#define PUZZLEPRESENTER_H

#include "InputMode.h"
#include "InputModeHandler.h"
#include "PathResolver.h"
#include "PuzzleEngine.h"
#include "UndoRedoManager.h"
#include "IGameSaver.h"
#include "IHeuristic.h"
#include "PuzzleConcepts.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

template<PuzzleTile T>
class MoveAdvisor;

/**
 * @brief Struktura zawierająca wszystkie dane potrzebne do wyrenderowania widoku
 *
 * Zgodnie ze wzorcem MVP, prezenter udostępnia widokowi jedną zintegrowaną
 * paczkę danych, dzięki czemu warstwa widoku pozostaje pasywna.
 */
struct PuzzleViewState {
    const Board<int>& board;
    const GameStats& stats;
    std::string statusMessage;
    InputMode inputMode;
    std::string inputBuffer;
    double heuristicValue;
    std::optional<std::pair<int, int>> hintPosition;
};

/**
 * @brief Klasa prezentera zarządzająca interakcją między silnikiem gry a UI
 *
 * Implementuje wzorzec MVP (Model-View-Presenter), koordynując logikę gry,
 * historię ruchów, operacje zapisu/odczytu stanu gry oraz stan UI
 * (komunikaty statusu, tryb wprowadzania ścieżki do plików, podświetlenie podpowiedzi).
 * Używa wzorca Strategy poprzez polimorficzny wskaźnik do IGameSaver oraz IHeuristic.
 */
class PuzzlePresenter {
private:
    PuzzleEngine<int> engine_;
    UndoRedoManager undoRedoManager_;
    std::unique_ptr<IGameSaver> saveHandler_;
    std::unique_ptr<IHeuristic<int>> heuristic_;

    InputModeHandler inputHandler_;
    std::string statusMessage_;
    std::string savePath_;
    std::optional<std::pair<int, int>> hintPosition_;
    std::function<void()> onChange_;

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

    /**
     * @brief Powiadamia obserwatora o zmianie stanu prezentera
     */
    void notifyChanged();

public:
    /**
     * @brief Konstruktor tworzący prezenter z domyślną strategią zapisu
     * @param size Rozmiar planszy (N x N)
     * @param saver Wskaźnik do implementacji IGameSaver (domyślnie TextGameSaver)
     * @param heuristic Wskaźnik do implementacji IHeuristic<int> (domyślnie ManhattanDistance<int>)
     * @param savePath Domyślna ścieżka do pliku zapisu
     */
    explicit PuzzlePresenter(int size,
                             std::unique_ptr<IGameSaver> saver = nullptr,
                             std::unique_ptr<IHeuristic<int>> heuristic = nullptr,
                             std::string savePath = "puzzle_save.txt");

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
     * @brief Wykonuje ruch w danym kierunku z obsługą historii i aktualizacją komunikatu
     * @param dir Kierunek ruchu
     * @return true jeśli ruch został wykonany, false w przeciwnym wypadku
     */
    bool move(Direction dir);

    /**
     * @brief Cofa ostatni ruch i aktualizuje komunikat statusu
     * @return true jeśli cofnięcie się powiodło, false w przeciwnym wypadku
     */
    bool undo();

    /**
     * @brief Ponawia cofnięty ruch i aktualizuje komunikat statusu
     * @return true jeśli ponowienie się powiodło, false w przeciwnym wypadku
     */
    bool redo();

    /**
     * @brief Tasuje planszę generując nową grę
     */
    void shuffle();

    /**
     * @brief Oblicza sugerowany ruch przy użyciu MoveAdvisor i ustawia podświetlenie
     * @param advisor Referencja do MoveAdvisor używanego do wyznaczenia ruchu
     */
    void hint(const MoveAdvisor<int>& advisor);

    /**
     * @brief Rozpoczyna tryb wprowadzania ścieżki do zapisu gry
     */
    void beginSaveMode();

    /**
     * @brief Rozpoczyna tryb wprowadzania ścieżki do wczytania gry
     */
    void beginLoadMode();

    /**
     * @brief Potwierdza aktualny tryb wejścia i wykonuje akcję zapisu/wczytania
     *
     * Normalizuje ścieżkę z bufora wejściowego, wykonuje odpowiednią operację
     * w zależności od aktywnego trybu (Save/Load) oraz aktualizuje komunikat statusu.
     * Wyczyści tryb wejścia (ustawia na None) niezależnie od sukcesu operacji.
     */
    void confirmInputMode();

    /**
     * @brief Anuluje aktualny tryb wejścia i czyści bufor wejściowy
     */
    void cancelInputMode();

    /**
     * @brief Dodaje znak do bufora wejściowego (tylko w trybie Save/Load)
     * @param c Znak do dodania
     */
    void handleCharacter(char c);

    /**
     * @brief Usuwa ostatni znak z bufora wejściowego (backspace)
     */
    void inputBackspace();

    /**
     * @brief Sprawdza czy aktywny jest tryb wprowadzania tekstu
     * @return true jeśli inputMode != None
     */
    bool isInInputMode() const noexcept;

    /**
     * @brief Pobiera aktualny tryb wejścia
     * @return Aktualny InputMode
     */
    InputMode getInputMode() const noexcept;

    /**
     * @brief Pobiera aktualny bufor wejściowy
     * @return Stała referencja do bufora
     */
    const std::string& getInputBuffer() const noexcept;

    /**
     * @brief Pobiera aktualny komunikat statusu
     * @return Stała referencja do komunikatu
     */
    const std::string& getStatusMessage() const noexcept;

    /**
     * @brief Ustawia komunikat statusu widoczny dla użytkownika
     * @param message Treść komunikatu
     */
    void setStatusMessage(std::string message);

    /**
     * @brief Pobiera aktualną ścieżkę zapisu
     * @return Stała referencja do ścieżki
     */
    const std::string& getSavePath() const noexcept;

    /**
     * @brief Ustawia ścieżkę do pliku zapisu
     * @param path Nowa ścieżka zapisu
     */
    void setSavePath(std::string path);

    /**
     * @brief Ustawia heurystykę używaną do obliczania wartości oceny planszy
     * @param heuristic Wskaźnik do implementacji IHeuristic<int>
     */
    void setHeuristic(std::unique_ptr<IHeuristic<int>> heuristic);

    /**
     * @brief Oblicza aktualną wartość heurystyki dla planszy
     * @return Wartość heurystyki lub 0.0 gdy heurystyka nie jest ustawiona
     */
    double getHeuristicValue() const;

    /**
     * @brief Pobiera pozycję kafelka do podświetlenia jako podpowiedź
     * @return Opcjonalna pozycja (x, y) lub std::nullopt gdy brak podpowiedzi
     */
    std::optional<std::pair<int, int>> getHintPosition() const noexcept;

    /**
     * @brief Czyści aktualne podświetlenie podpowiedzi
     */
    void clearHintHighlight();

    /**
     * @brief Rejestruje callback wywoływany przy każdej zmianie stanu widoku
     * @param callback Funkcja wywoływana po mutacji stanu (np. do odświeżenia ekranu)
     */
    void setOnChange(std::function<void()> callback);

    /**
     * @brief Inicjalizuje stan początkowy prezentera
     *
     * Gdy autoLoadSave == true i plik istnieje — wczytuje grę; w przeciwnym razie tasuje
     * planszę i ustawia odpowiedni komunikat statusu.
     *
     * @param autoLoadSave Jeśli true, próbuj wczytać grę z bieżącej ścieżki zapisu
     */
    void bootstrap(bool autoLoadSave);

    /**
     * @brief Gromadzi kompletny zestaw danych wymagany przez widok
     * @return Struktura PuzzleViewState z referencjami do aktualnego stanu gry
     */
    PuzzleViewState getViewState() const;

    /**
     * @brief Normalizuje ścieżkę - rozszerza ~ na home directory i obsługuje ścieżki absolutne
     * @param path Ścieżka do normalizacji
     * @return Znormalizowana ścieżka
     */
    static std::string normalizePath(const std::string& path) {
        return PathResolver::normalizePath(path);
    }

    /**
     * @brief Sprawdza czy plik istnieje w katalogu bieżącym lub nadrzędnym
     * @param filename Nazwa pliku do sprawdzenia
     * @return Ścieżka do pliku jeśli istnieje, pusty string w przeciwnym wypadku
     */
    static std::string findSaveFile(const std::string& filename) {
        return PathResolver::findSaveFile(filename);
    }

    /**
     * @brief Wykrywa istniejący plik zapisu w bieżącym lub nadrzędnym katalogu
     * @param filename Nazwa poszukiwanego pliku (np. "puzzle_save.txt")
     * @return std::optional ze ścieżką do pliku lub std::nullopt gdy plik nie istnieje
     */
    static std::optional<std::string> detectSaveFile(const std::string& filename) {
        return PathResolver::detectSaveFile(filename);
    }

    /**
     * @brief Odczytuje rozmiar planszy zapisanej w pierwszej linii pliku zapisu
     * @param filename Ścieżka do pliku zapisu
     * @return std::optional z rozmiarem planszy lub std::nullopt gdy odczyt się nie powiódł
     */
    static std::optional<int> probeSavedBoardSize(const std::string& filename) {
        return PathResolver::probeSavedBoardSize(filename);
    }
};

#endif
