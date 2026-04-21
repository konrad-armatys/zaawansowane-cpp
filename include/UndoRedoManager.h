#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include <stack>
#include <optional>
#include <utility>

/**
 * @brief Struktura reprezentująca pojedynczy ruch
 *
 * Przechowuje tylko współrzędne źródłowe i docelowe ruchu,
 * co jest znacznie bardziej efektywne niż przechowywanie całego stanu planszy.
 */
struct MoveData {
    int fromX;
    int fromY;
    int toX;
    int toY;

    /**
     * @brief Konstruktor ruchu
     * @param fx Współrzędna x źródła
     * @param fy Współrzędna y źródła
     * @param tx Współrzędna x celu
     * @param ty Współrzędna y celu
     */
    MoveData(int fx, int fy, int tx, int ty)
        : fromX(fx), fromY(fy), toX(tx), toY(ty) {}

    /**
     * @brief Tworzy odwrotny ruch (do cofania)
     * @return Ruch w przeciwnym kierunku
     */
    [[nodiscard]] MoveData reverse() const noexcept {
        return MoveData(toX, toY, fromX, fromY);
    }

    /**
     * @brief Operator porównania
     * @param other Inny ruch
     * @return true jeśli ruchy są identyczne
     */
    [[nodiscard]] bool operator==(const MoveData& other) const noexcept {
        return fromX == other.fromX && fromY == other.fromY &&
               toX == other.toX && toY == other.toY;
    }
};

/**
 * @brief Menedżer historii ruchów z mechanizmem Undo/Redo
 *
 * Implementuje klasyczny wzorzec dwóch stosów:
 * - Stos A (undoStack_): Historia wykonanych ruchów
 * - Stos B (redoStack_): Cofnięte ruchy gotowe do ponowienia
 *
 * Kluczowe zasady:
 * 1. Nowy ruch trafia na stos A i czyści stos B
 * 2. Undo: zdejmuje ze stosu A, kładzie na stos B
 * 3. Redo: zdejmuje ze stosu B, kładzie na stos A
 */
class UndoRedoManager {
private:
    std::stack<MoveData> undoStack_;
    std::stack<MoveData> redoStack_;
    size_t maxHistorySize_;

    /**
     * @brief Ogranicza rozmiar stosu do maksymalnej wartości
     * @param stack Stos do ograniczenia
     */
    void limitStackSize(std::stack<MoveData>& stack);

public:
    /**
     * @brief Konstruktor z opcjonalnym limitem historii
     * @param maxHistorySize Maksymalny rozmiar historii (0 = nielimitowany)
     */
    explicit UndoRedoManager(size_t maxHistorySize = 100);

    /**
     * @brief Zapisuje nowy ruch do historii (na stos A)
     *
     * WAŻNE: Automatycznie czyści stos B (Redo), ponieważ nowy ruch
     * unieważnia poprzednio cofniętą historię.
     *
     * @param fromX Współrzędna x źródła
     * @param fromY Współrzędna y źródła
     * @param toX Współrzędna x celu
     * @param toY Współrzędna y celu
     */
    void pushMove(int fromX, int fromY, int toX, int toY);

    /**
     * @brief Cofa ostatni ruch (Undo)
     *
     * Przenosi ruch ze stosu A na stos B i zwraca odwrócony ruch
     * do wykonania (aby cofnąć efekt).
     *
     * @return Odwrócony ruch do wykonania lub std::nullopt jeśli brak historii
     */
    [[nodiscard]] std::optional<MoveData> undo();

    /**
     * @brief Przywraca cofnięty ruch (Redo)
     *
     * Przenosi ruch ze stosu B na stos A i zwraca ruch do wykonania.
     *
     * @return Ruch do ponownego wykonania lub std::nullopt jeśli brak cofniętych ruchów
     */
    [[nodiscard]] std::optional<MoveData> redo();

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
     * @brief Czyści całą historię Undo i Redo (oba stosy)
     */
    void clear();

    /**
     * @brief Czyści tylko stos Redo (stos B)
     *
     * Wywoływane automatycznie przy nowym ruchu.
     */
    void clearRedoStack();

    /**
     * @brief Zwraca rozmiar stosu Undo (stos A)
     * @return Liczba ruchów w historii Undo
     */
    [[nodiscard]] size_t getUndoStackSize() const noexcept;

    /**
     * @brief Zwraca rozmiar stosu Redo (stos B)
     * @return Liczba ruchów w historii Redo
     */
    [[nodiscard]] size_t getRedoStackSize() const noexcept;

    /**
     * @brief Ustawia maksymalny rozmiar historii
     * @param maxSize Nowy maksymalny rozmiar (0 = nielimitowany)
     */
    void setMaxHistorySize(size_t maxSize);

    /**
     * @brief Pobiera maksymalny rozmiar historii
     * @return Maksymalny rozmiar lub 0 jeśli nielimitowany
     */
    [[nodiscard]] size_t getMaxHistorySize() const noexcept {
        return maxHistorySize_;
    }
};

#endif
