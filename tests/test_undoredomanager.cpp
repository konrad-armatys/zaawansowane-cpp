#include "UndoRedoManager.h"
#include <iostream>
#include <cassert>

/**
 * @brief Test konstruktora i podstawowych getterów
 */
void testConstructor() {
    std::cout << "Test: Constructor... ";

    UndoRedoManager manager(50);
    assert(manager.getMaxHistorySize() == 50);
    assert(manager.getUndoStackSize() == 0);
    assert(manager.getRedoStackSize() == 0);
    assert(!manager.canUndo());
    assert(!manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test dodawania ruchów na stos A
 */
void testPushMove() {
    std::cout << "Test: Push moves... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);
    assert(manager.getUndoStackSize() == 1);
    assert(manager.canUndo());
    assert(!manager.canRedo());

    manager.pushMove(1, 0, 1, 1);
    assert(manager.getUndoStackSize() == 2);

    std::cout << "PASSED\n";
}

/**
 * @brief Test podstawowego Undo
 */
void testBasicUndo() {
    std::cout << "Test: Basic undo... ";

    UndoRedoManager manager;


    manager.pushMove(0, 0, 1, 0);


    auto undoMove = manager.undo();
    assert(undoMove.has_value());


    assert(undoMove->fromX == 1 && undoMove->fromY == 0);
    assert(undoMove->toX == 0 && undoMove->toY == 0);


    assert(manager.getUndoStackSize() == 0);
    assert(manager.getRedoStackSize() == 1);
    assert(!manager.canUndo());
    assert(manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test podstawowego Redo
 */
void testBasicRedo() {
    std::cout << "Test: Basic redo... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);
    (void)manager.undo();


    auto redoMove = manager.redo();
    assert(redoMove.has_value());


    assert(redoMove->fromX == 0 && redoMove->fromY == 0);
    assert(redoMove->toX == 1 && redoMove->toY == 0);


    assert(manager.getUndoStackSize() == 1);
    assert(manager.getRedoStackSize() == 0);
    assert(manager.canUndo());
    assert(!manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test że nowy ruch czyści stos Redo
 */
void testNewMoveClearsRedo() {
    std::cout << "Test: New move clears redo stack... ";

    UndoRedoManager manager;


    manager.pushMove(0, 0, 1, 0);
    manager.pushMove(1, 0, 1, 1);
    manager.pushMove(1, 1, 0, 1);


    (void)manager.undo();
    (void)manager.undo();

    assert(manager.getUndoStackSize() == 1);
    assert(manager.getRedoStackSize() == 2);


    manager.pushMove(1, 0, 2, 0);

    assert(manager.getUndoStackSize() == 2);
    assert(manager.getRedoStackSize() == 0);
    assert(!manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test sekwencji Undo/Redo
 */
void testUndoRedoSequence() {
    std::cout << "Test: Undo/Redo sequence... ";

    UndoRedoManager manager;


    manager.pushMove(0, 0, 1, 0);
    manager.pushMove(1, 0, 1, 1);
    manager.pushMove(1, 1, 0, 1);
    manager.pushMove(0, 1, 0, 2);
    manager.pushMove(0, 2, 1, 2);

    assert(manager.getUndoStackSize() == 5);


    (void)manager.undo();
    (void)manager.undo();
    (void)manager.undo();

    assert(manager.getUndoStackSize() == 2);
    assert(manager.getRedoStackSize() == 3);


    (void)manager.redo();
    (void)manager.redo();

    assert(manager.getUndoStackSize() == 4);
    assert(manager.getRedoStackSize() == 1);

    std::cout << "PASSED\n";
}

/**
 * @brief Test cofania gdy stos jest pusty
 */
void testUndoWhenEmpty() {
    std::cout << "Test: Undo when empty... ";

    UndoRedoManager manager;

    auto result = manager.undo();
    assert(!result.has_value());
    assert(!manager.canUndo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test przywracania gdy stos Redo jest pusty
 */
void testRedoWhenEmpty() {
    std::cout << "Test: Redo when empty... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);

    auto result = manager.redo();
    assert(!result.has_value());
    assert(!manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test czyszczenia całej historii
 */
void testClear() {
    std::cout << "Test: Clear history... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);
    manager.pushMove(1, 0, 1, 1);
    manager.pushMove(1, 1, 0, 1);
    (void)manager.undo();
    (void)manager.undo();

    assert(manager.getUndoStackSize() == 1);
    assert(manager.getRedoStackSize() == 2);

    manager.clear();

    assert(manager.getUndoStackSize() == 0);
    assert(manager.getRedoStackSize() == 0);
    assert(!manager.canUndo());
    assert(!manager.canRedo());

    std::cout << "PASSED\n";
}

/**
 * @brief Test limitu historii
 */
void testHistoryLimit() {
    std::cout << "Test: History limit... ";

    UndoRedoManager manager(3);


    manager.pushMove(0, 0, 1, 0);
    manager.pushMove(1, 0, 1, 1);
    manager.pushMove(1, 1, 0, 1);
    manager.pushMove(0, 1, 0, 2);
    manager.pushMove(0, 2, 1, 2);

    assert(manager.getUndoStackSize() == 3);

    std::cout << "PASSED\n";
}

/**
 * @brief Test zmiany limitu historii
 */
void testChangeHistoryLimit() {
    std::cout << "Test: Change history limit... ";

    UndoRedoManager manager(100);

    for (int i = 0; i < 10; ++i) {
        manager.pushMove(i, 0, i+1, 0);
    }

    assert(manager.getUndoStackSize() == 10);


    manager.setMaxHistorySize(5);

    assert(manager.getUndoStackSize() == 5);
    assert(manager.getMaxHistorySize() == 5);

    std::cout << "PASSED\n";
}

/**
 * @brief Test odwracania ruchu
 */
void testMoveReverse() {
    std::cout << "Test: Move reverse... ";

    MoveData move(1, 2, 3, 4);
    MoveData reversed = move.reverse();

    assert(reversed.fromX == 3 && reversed.fromY == 4);
    assert(reversed.toX == 1 && reversed.toY == 2);

    std::cout << "PASSED\n";
}

/**
 * @brief Test operatora porównania MoveData
 */
void testMoveEquality() {
    std::cout << "Test: Move equality... ";

    MoveData move1(1, 2, 3, 4);
    MoveData move2(1, 2, 3, 4);
    MoveData move3(1, 2, 3, 5);

    assert(move1 == move2);
    assert(!(move1 == move3));

    std::cout << "PASSED\n";
}

/**
 * @brief Test wielokrotnego Undo/Redo na tym samym ruchu
 */
void testMultipleUndoRedo() {
    std::cout << "Test: Multiple undo/redo cycles... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);


    for (int i = 0; i < 5; ++i) {
        auto undoMove = manager.undo();
        assert(undoMove.has_value());
        assert(manager.getRedoStackSize() == 1);

        auto redoMove = manager.redo();
        assert(redoMove.has_value());
        assert(manager.getUndoStackSize() == 1);
    }

    std::cout << "PASSED\n";
}

/**
 * @brief Test czyszczenia tylko stosu Redo
 */
void testClearRedoStack() {
    std::cout << "Test: Clear redo stack only... ";

    UndoRedoManager manager;

    manager.pushMove(0, 0, 1, 0);
    manager.pushMove(1, 0, 1, 1);
    (void)manager.undo();
    (void)manager.undo();

    assert(manager.getUndoStackSize() == 0);
    assert(manager.getRedoStackSize() == 2);

    manager.clearRedoStack();

    assert(manager.getUndoStackSize() == 0);
    assert(manager.getRedoStackSize() == 0);

    std::cout << "PASSED\n";
}

/**
 * @brief Test z nielimitowaną historią
 */
void testUnlimitedHistory() {
    std::cout << "Test: Unlimited history... ";

    UndoRedoManager manager(0);


    for (int i = 0; i < 200; ++i) {
        manager.pushMove(i, 0, i+1, 0);
    }


    assert(manager.getUndoStackSize() == 200);

    std::cout << "PASSED\n";
}

/**
 * @brief Funkcja główna uruchamiająca wszystkie testy
 */
int main() {
    std::cout << "=== UndoRedoManager Tests ===\n\n";

    std::cout << "--- Basic operations ---\n";
    testConstructor();
    testPushMove();
    testBasicUndo();
    testBasicRedo();
    testMoveReverse();
    testMoveEquality();

    std::cout << "\n--- Undo/Redo logic ---\n";
    testNewMoveClearsRedo();
    testUndoRedoSequence();
    testMultipleUndoRedo();

    std::cout << "\n--- Edge cases ---\n";
    testUndoWhenEmpty();
    testRedoWhenEmpty();
    testClear();
    testClearRedoStack();

    std::cout << "\n--- History management ---\n";
    testHistoryLimit();
    testChangeHistoryLimit();
    testUnlimitedHistory();

    std::cout << "\n=== All 17 tests passed! ===\n";
    return 0;
}
