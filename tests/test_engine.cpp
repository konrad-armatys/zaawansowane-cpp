#include "PuzzleEngine.h"
#include <iostream>
#include <cassert>

/**
 * @brief Wyświetla planszę w konsoli
 */
void printBoard(const Board<int>& board) {
    const int size = board.getSize();
    std::cout << "\nBoard state:\n";
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int val = board.at(x, y);
            if (val == 0) {
                std::cout << "  . ";
            } else {
                std::cout << (val < 10 ? "  " : " ") << val << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

/**
 * @brief Test podstawowych funkcji silnika
 */
void testBasicEngine() {
    std::cout << "=== Test 1: Podstawowa funkcjonalność ===" << std::endl;

    PuzzleEngine<int> engine(3);


    engine.reset(0);
    printBoard(engine.getBoard());
    assert(engine.isSolved(0));
    std::cout << "✓ Reset creates solved board" << std::endl;


    bool moved = engine.move(Direction::Up, 0);
    assert(moved);
    assert(!engine.isSolved(0));
    std::cout << "✓ Move executed successfully" << std::endl;
    printBoard(engine.getBoard());


    assert(engine.getStats().movesCount.get() == 1);
    std::cout << "✓ Stats updated after move (moves: "
              << engine.getStats().movesCount.get() << ")" << std::endl;
}

/**
 * @brief Test tasowania i rozwiązywalności
 */
void testShuffle() {
    std::cout << "\n=== Test 2: Tasowanie i rozwiązywalność ===" << std::endl;

    PuzzleEngine<int> engine(3);


    for (int i = 0; i < 5; ++i) {
        engine.shuffle(0);
        printBoard(engine.getBoard());





        std::cout << "Shuffle " << (i + 1) << " - Correct tiles: "
                  << engine.getStats().correctTiles.get() << "/"
                  << (engine.getBoard().getSize() * engine.getBoard().getSize())
                  << std::endl;
    }

    std::cout << "✓ Shuffle executed multiple times" << std::endl;
}

/**
 * @brief Test granic planszy
 */
void testBoundaries() {
    std::cout << "\n=== Test 3: Granice planszy ===" << std::endl;

    PuzzleEngine<int> engine(3);
    engine.reset(0);


    auto [x, y] = engine.getEmptyPosition();
    std::cout << "Empty tile position: (" << x << ", " << y << ")" << std::endl;
    assert(x == 2 && y == 2);


    bool movedRight = engine.move(Direction::Right, 0);
    bool movedDown = engine.move(Direction::Down, 0);
    assert(!movedRight && !movedDown);
    std::cout << "✓ Boundary moves correctly blocked" << std::endl;


    bool movedLeft = engine.move(Direction::Left, 0);
    assert(movedLeft);
    std::cout << "✓ Valid moves work correctly" << std::endl;
}

/**
 * @brief Test różnych rozmiarów planszy
 */
void testDifferentSizes() {
    std::cout << "\n=== Test 4: Różne rozmiary planszy ===" << std::endl;

    for (int size : {2, 3, 4, 5}) {
        PuzzleEngine<int> engine(size);
        engine.reset(0);

        assert(engine.isSolved(0));
        std::cout << "✓ " << size << "x" << size << " board initialized correctly" << std::endl;

        engine.shuffle(0);
        std::cout << "  Shuffled " << size << "x" << size
                  << " - Correct tiles: " << engine.getStats().correctTiles.get()
                  << "/" << (size * size) << std::endl;
    }
}

int main() {
    try {
        testBasicEngine();
        testShuffle();
        testBoundaries();
        testDifferentSizes();

        std::cout << "\n=== Wszystkie testy zakończone sukcesem! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
