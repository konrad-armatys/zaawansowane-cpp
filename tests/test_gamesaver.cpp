#include "BinaryGameSaver.h"
#include "GameState.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

/**
 * @brief Test podstawowego zapisu i odczytu
 */
void testBasicSaveLoad() {
    std::cout << "\n=== Test 1: Podstawowy zapis i odczyt ===" << std::endl;


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


    BinaryGameSaver saver;
    const std::string testFile = "test_save.npuz";

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
        std::cout << "✓ Plansza zgodna" << std::endl;


        assert(loadedState.stats.movesCount.get() == 10);
        assert(loadedState.stats.undoCount.get() == 2);
        assert(loadedState.stats.correctTiles.get() == 8);
        std::cout << "✓ Statystyki zgodne" << std::endl;


        assert(loadedState.undoHistory.size() == 2);
        assert(loadedState.undoHistory[0] == undoHistory[0]);
        assert(loadedState.undoHistory[1] == undoHistory[1]);
        std::cout << "✓ Historia Undo zgodna" << std::endl;


        assert(loadedState.redoHistory.size() == 1);
        assert(loadedState.redoHistory[0] == redoHistory[0]);
        std::cout << "✓ Historia Redo zgodna" << std::endl;


        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::filesystem::remove(testFile);
        throw;
    }
}

/**
 * @brief Test pustej historii
 */
void testEmptyHistory() {
    std::cout << "\n=== Test 2: Pusta historia ===" << std::endl;

    Board<int> board(3);
    GameStats stats(3);
    GameState state(board, stats);

    BinaryGameSaver saver;
    const std::string testFile = "test_empty.npuz";

    try {
        saver.save(testFile, state);
        GameState loaded = saver.load(testFile);

        assert(loaded.undoHistory.empty());
        assert(loaded.redoHistory.empty());
        std::cout << "✓ Pusta historia poprawnie zapisana i wczytana" << std::endl;

        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::filesystem::remove(testFile);
        throw;
    }
}

/**
 * @brief Test dużej historii
 */
void testLargeHistory() {
    std::cout << "\n=== Test 3: Duża historia ===" << std::endl;

    Board<int> board(4);
    GameStats stats(4);

    std::vector<MoveData> undoHistory;
    for (int i = 0; i < 100; ++i) {
        undoHistory.emplace_back(i % 4, i % 4, (i + 1) % 4, (i + 1) % 4);
    }

    std::vector<MoveData> redoHistory;
    for (int i = 0; i < 50; ++i) {
        redoHistory.emplace_back(i % 4, i % 4, (i + 2) % 4, (i + 2) % 4);
    }

    GameState state(board, stats, undoHistory, redoHistory);

    BinaryGameSaver saver;
    const std::string testFile = "test_large.npuz";

    try {
        saver.save(testFile, state);
        GameState loaded = saver.load(testFile);

        assert(loaded.undoHistory.size() == 100);
        assert(loaded.redoHistory.size() == 50);

        for (size_t i = 0; i < undoHistory.size(); ++i) {
            assert(loaded.undoHistory[i] == undoHistory[i]);
        }

        for (size_t i = 0; i < redoHistory.size(); ++i) {
            assert(loaded.redoHistory[i] == redoHistory[i]);
        }

        std::cout << "✓ Duża historia poprawnie zapisana i wczytana" << std::endl;

        std::filesystem::remove(testFile);
    } catch (const std::exception& e) {
        std::filesystem::remove(testFile);
        throw;
    }
}

/**
 * @brief Test nieprawidłowego pliku
 */
void testInvalidFile() {
    std::cout << "\n=== Test 4: Nieprawidłowy plik ===" << std::endl;

    BinaryGameSaver saver;

    try {
        saver.load("nonexistent_file.npuz");
        assert(false && "Powinien zgłosić wyjątek");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ Poprawnie zgłoszono wyjątek dla nieistniejącego pliku" << std::endl;
    }


    const std::string badFile = "test_bad.npuz";
    std::ofstream ofs(badFile, std::ios::binary);
    ofs << "invalid data";
    ofs.close();

    try {
        saver.load(badFile);
        assert(false && "Powinien zgłosić wyjątek");
    } catch (const std::exception& e) {
        std::cout << "✓ Poprawnie zgłoszono wyjątek dla nieprawidłowego formatu" << std::endl;
    }

    std::filesystem::remove(badFile);
}

/**
 * @brief Test różnych rozmiarów planszy
 */
void testDifferentBoardSizes() {
    std::cout << "\n=== Test 5: Różne rozmiary planszy ===" << std::endl;

    BinaryGameSaver saver;

    for (int size : {3, 4, 5}) {
        Board<int> board(size);
        GameStats stats(size);
        GameState state(board, stats);

        const std::string testFile = "test_size_" + std::to_string(size) + ".npuz";

        try {
            saver.save(testFile, state);
            GameState loaded = saver.load(testFile);

            assert(loaded.board.getSize() == size);
            std::cout << "✓ Plansza " << size << "x" << size << " poprawnie zapisana i wczytana" << std::endl;

            std::filesystem::remove(testFile);
        } catch (const std::exception& e) {
            std::filesystem::remove(testFile);
            throw;
        }
    }
}

int main() {
    try {
        std::cout << "===========================================" << std::endl;
        std::cout << "  TESTY SYSTEMU ZAPISU/ODCZYTU GRY" << std::endl;
        std::cout << "===========================================" << std::endl;

        testBasicSaveLoad();
        testEmptyHistory();
        testLargeHistory();
        testInvalidFile();
        testDifferentBoardSizes();

        std::cout << "\n===========================================" << std::endl;
        std::cout << "  WSZYSTKIE TESTY ZAKOŃCZONE SUKCESEM ✓" << std::endl;
        std::cout << "===========================================" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ BŁĄD: " << e.what() << std::endl;
        return 1;
    }
}
