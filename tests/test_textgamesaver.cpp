#include "TextGameSaver.h"
#include "GameState.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

/**
 * @brief Test podstawowego zapisu i odczytu w formacie tekstowym
 */
void testBasicSaveLoad() {
    std::cout << "\n=== Test 1: Podstawowy zapis i odczyt (format .txt) ===" << std::endl;


    Board<int> board(3, {1, 2, 3, 4, 5, 6, 7, 8, 0});
    GameStats stats(3);
    stats.movesCount.set(10);
    stats.undoCount.set(2);
    stats.correctTiles.set(8);

    std::vector<MoveData> undoHistory = {
        MoveData(0, 0, 1, 0),
        MoveData(1, 0, 2, 0)
    };

    std::vector<MoveData> redoHistory = {
        MoveData(2, 0, 1, 0)
    };

    GameState originalState(board, stats, undoHistory, redoHistory);


    TextGameSaver saver;
    const std::string testFile = "test_save.txt";

    try {
        saver.save(testFile, originalState);
        std::cout << "✓ Zapis zakończony sukcesem" << std::endl;


        GameState loadedState = saver.load(testFile);
        std::cout << "✓ Odczyt zakończony sukcesem" << std::endl;


        assert(loadedState.board.getSize() == originalState.board.getSize());
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                assert(loadedState.board.at(x, y) == originalState.board.at(x, y));
            }
        }
        std::cout << "✓ Plansza zweryfikowana poprawnie" << std::endl;


        assert(loadedState.stats.movesCount.get() == originalState.stats.movesCount.get());
        assert(loadedState.stats.undoCount.get() == originalState.stats.undoCount.get());
        assert(loadedState.stats.correctTiles.get() == originalState.stats.correctTiles.get());
        std::cout << "✓ Statystyki zweryfikowane poprawnie" << std::endl;


        assert(loadedState.undoHistory.size() == originalState.undoHistory.size());
        for (size_t i = 0; i < loadedState.undoHistory.size(); ++i) {
            assert(loadedState.undoHistory[i] == originalState.undoHistory[i]);
        }
        std::cout << "✓ Historia Undo zweryfikowana poprawnie" << std::endl;

        assert(loadedState.redoHistory.size() == originalState.redoHistory.size());
        for (size_t i = 0; i < loadedState.redoHistory.size(); ++i) {
            assert(loadedState.redoHistory[i] == originalState.redoHistory[i]);
        }
        std::cout << "✓ Historia Redo zweryfikowana poprawnie" << std::endl;


        std::filesystem::remove(testFile);

    } catch (const std::exception& e) {
        std::cerr << "✗ Test nie powiódł się: " << e.what() << std::endl;
        throw;
    }

    std::cout << "✓ Test 1 zakończony sukcesem" << std::endl;
}

/**
 * @brief Test odczytu nieistniejącego pliku
 */
void testNonexistentFile() {
    std::cout << "\n=== Test 2: Odczyt nieistniejącego pliku ===" << std::endl;

    TextGameSaver saver;
    bool exceptionThrown = false;

    try {
        saver.load("nonexistent_file_xyz123.txt");
    } catch (const std::runtime_error& e) {
        exceptionThrown = true;
        std::cout << "✓ Poprawnie rzucony wyjątek: " << e.what() << std::endl;
    }

    assert(exceptionThrown);
    std::cout << "✓ Test 2 zakończony sukcesem" << std::endl;
}

/**
 * @brief Test odczytu uszkodzonego pliku
 */
void testCorruptedFile() {
    std::cout << "\n=== Test 3: Odczyt uszkodzonego pliku ===" << std::endl;

    const std::string testFile = "test_corrupted.txt";


    std::ofstream file(testFile);
    file << "3\n";
    file << "1 2 3 4 5\n";
    file.close();

    TextGameSaver saver;
    bool exceptionThrown = false;

    try {
        saver.load(testFile);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
        std::cout << "✓ Poprawnie rzucony wyjątek: " << e.what() << std::endl;
    }

    assert(exceptionThrown);
    std::filesystem::remove(testFile);
    std::cout << "✓ Test 3 zakończony sukcesem" << std::endl;
}

/**
 * @brief Test z pustymi stosami historii
 */
void testEmptyHistory() {
    std::cout << "\n=== Test 4: Puste stosy historii ===" << std::endl;

    Board<int> board(4, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0});
    GameStats stats(4);
    GameState originalState(board, stats, {}, {});

    TextGameSaver saver;
    const std::string testFile = "test_empty_history.txt";

    try {
        saver.save(testFile, originalState);
        GameState loadedState = saver.load(testFile);

        assert(loadedState.undoHistory.empty());
        assert(loadedState.redoHistory.empty());
        std::cout << "✓ Puste stosy historii obsłużone poprawnie" << std::endl;

        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::cerr << "✗ Test nie powiódł się: " << e.what() << std::endl;
        throw;
    }

    std::cout << "✓ Test 4 zakończony sukcesem" << std::endl;
}

/**
 * @brief Test dużej planszy (101x101)
 */
void testLargeBoard() {
    std::cout << "\n=== Test 5: Duża plansza 101x101 ===" << std::endl;

    std::vector<int> tiles(101 * 101);
    for (int i = 0; i < 101 * 101 - 1; ++i) {
        tiles[i] = i + 1;
    }
    tiles[101 * 101 - 1] = 0;

    Board<int> board(101, tiles);
    GameStats stats(101);
    GameState originalState(board, stats);

    TextGameSaver saver;
    const std::string testFile = "test_large.txt";

    try {
        saver.save(testFile, originalState);
        std::cout << "✓ Zapis dużej planszy zakończony sukcesem" << std::endl;

        GameState loadedState = saver.load(testFile);
        std::cout << "✓ Odczyt dużej planszy zakończony sukcesem" << std::endl;

        assert(loadedState.board.getSize() == 101);


        assert(loadedState.board.at(0, 0) == 1);
        assert(loadedState.board.at(50, 50) == 50 * 101 + 51);
        assert(loadedState.board.at(100, 100) == 0);

        std::cout << "✓ Duża plansza zweryfikowana poprawnie" << std::endl;

        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::cerr << "✗ Test nie powiódł się: " << e.what() << std::endl;
        throw;
    }

    std::cout << "✓ Test 5 zakończony sukcesem" << std::endl;
}

/**
 * @brief Test formatu 4-liniowego
 */
void testFourLineFormat() {
    std::cout << "\n=== Test 6: Weryfikacja formatu 4-liniowego ===" << std::endl;

    Board<int> board(3, {1, 2, 3, 4, 5, 6, 7, 8, 0});
    GameStats stats(3);
    stats.movesCount.set(5);
    stats.undoCount.set(2);
    stats.correctTiles.set(8);

    std::vector<MoveData> undoHistory = {MoveData(0, 0, 1, 0)};
    std::vector<MoveData> redoHistory = {MoveData(1, 0, 2, 0)};

    GameState state(board, stats, undoHistory, redoHistory);

    TextGameSaver saver;
    const std::string testFile = "test_format.txt";

    try {
        saver.save(testFile, state);


        std::ifstream file(testFile);
        int lineCount = 0;
        std::string line;
        while (std::getline(file, line)) {
            ++lineCount;
        }
        file.close();

        assert(lineCount == 4);
        std::cout << "✓ Plik ma poprawny format 4-liniowy" << std::endl;

        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::cerr << "✗ Test nie powiódł się: " << e.what() << std::endl;
        throw;
    }

    std::cout << "✓ Test 6 zakończony sukcesem" << std::endl;
}

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "  TextGameSaver - Testy jednostkowe" << std::endl;
    std::cout << "=====================================" << std::endl;

    try {
        testBasicSaveLoad();
        testNonexistentFile();
        testCorruptedFile();
        testEmptyHistory();
        testLargeBoard();
        testFourLineFormat();

        std::cout << "\n=====================================" << std::endl;
        std::cout << "✓ WSZYSTKIE TESTY ZAKOŃCZONE SUKCESEM" << std::endl;
        std::cout << "=====================================" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n=====================================" << std::endl;
        std::cerr << "✗ TESTY NIEUDANE" << std::endl;
        std::cerr << "=====================================" << std::endl;
        return 1;
    }
}
