#include "TuiRenderer.h"
#include "ManhattanDistance.h"
#include "PuzzleEngine.h"
#include <iostream>
#include <cassert>

/**
 * @brief Test podstawowych funkcji TuiRenderer
 */
void testBasicRendering() {
    const int boardSize = 3;
    TuiRenderer renderer(boardSize);

    Board<int> board(boardSize);
    board.at(0, 0) = 1;
    board.at(1, 0) = 2;
    board.at(2, 0) = 3;
    board.at(0, 1) = 4;
    board.at(1, 1) = 5;
    board.at(2, 1) = 6;
    board.at(0, 2) = 7;
    board.at(1, 2) = 8;
    board.at(2, 2) = 0;

    std::cout << "Test 1: Podstawowe renderowanie 3x3" << std::endl;
    renderer.render(board);
    std::cout << "✓ Renderowanie zakończone bez błędów\n" << std::endl;
}

/**
 * @brief Test aktualizacji statystyk
 */
void testStatsUpdate() {
    const int boardSize = 3;
    TuiRenderer renderer(boardSize, std::make_unique<ManhattanDistance>());

    GameStats stats(boardSize);
    stats.movesCount.set(10);
    stats.undoCount.set(2);
    stats.correctTiles.set(7);

    Board<int> board(boardSize);
    for (int i = 0; i < boardSize * boardSize - 1; ++i) {
        board.at(i % boardSize, i / boardSize) = i + 1;
    }
    board.at(boardSize - 1, boardSize - 1) = 0;

    std::cout << "Test 2: Aktualizacja statystyk" << std::endl;
    renderer.updateStats(stats, 4.5);
    renderer.render(board);
    std::cout << "✓ Statystyki zaktualizowane poprawnie\n" << std::endl;
}

/**
 * @brief Test wyświetlania komunikatów
 */
void testMessageDisplay() {
    const int boardSize = 3;
    TuiRenderer renderer(boardSize);

    Board<int> board(boardSize);
    for (int i = 0; i < boardSize * boardSize - 1; ++i) {
        board.at(i % boardSize, i / boardSize) = i + 1;
    }
    board.at(boardSize - 1, boardSize - 1) = 0;

    std::cout << "Test 3: Wyświetlanie komunikatu" << std::endl;
    renderer.showMessage("Test message: Gra w toku!");
    renderer.render(board);
    std::cout << "✓ Komunikat wyświetlony poprawnie\n" << std::endl;
}

/**
 * @brief Test renderowania większej planszy
 */
void testLargeBoard() {
    const int boardSize = 5;
    TuiRenderer renderer(boardSize);

    Board<int> board(boardSize);
    int value = 1;
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            if (y == boardSize - 1 && x == boardSize - 1) {
                board.at(x, y) = 0;
            } else {
                board.at(x, y) = value++;
            }
        }
    }

    std::cout << "Test 4: Renderowanie większej planszy 5x5" << std::endl;
    renderer.render(board);
    std::cout << "✓ Duża plansza wyrenderowana poprawnie\n" << std::endl;
}

/**
 * @brief Test z heurystyką
 */
void testWithHeuristic() {
    const int boardSize = 4;
    TuiRenderer renderer(boardSize, std::make_unique<ManhattanDistance>());

    PuzzleEngine<int> engine(boardSize);
    engine.shuffle();

    const auto& board = engine.getBoard();
    const auto& stats = engine.getStats();

    std::cout << "Test 5: Renderowanie z heurystyką Manhattan Distance" << std::endl;
    auto heuristic = std::make_unique<ManhattanDistance>();
    renderer.updateStats(stats, heuristic->calculate(board));
    renderer.showMessage("Plansza potasowana - wartość heurystyki wyświetlona");
    renderer.render(board);
    std::cout << "✓ Heurystyka obliczona i wyświetlona poprawnie\n" << std::endl;
}

int main() {
    std::cout << "=== Testy TuiRenderer ===\n" << std::endl;

    try {
        testBasicRendering();
        testStatsUpdate();
        testMessageDisplay();
        testLargeBoard();
        testWithHeuristic();

        std::cout << "\n=== Wszystkie testy zakończone sukcesem! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test zakończony niepowodzeniem: " << e.what() << std::endl;
        return 1;
    }
}
