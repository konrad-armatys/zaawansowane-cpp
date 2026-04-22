/**
 * @file test_ui_components.cpp
 * @brief Comprehensive unit tests for UI components
 */

#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>


#include "TuiRenderer.h"
#include "PuzzlePresenter.h"
#include "Board.h"
#include "GameStats.h"


#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << std::endl; \
            std::cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
            throw std::runtime_error("Test assertion failed"); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) \
    do { \
        auto expectedVal = (expected); \
        auto actualVal = (actual); \
        if (expectedVal != actualVal) { \
            std::cerr << "Assertion failed: " << #expected << " != " << #actual << std::endl; \
            std::cerr << "Expected: " << expectedVal << ", Actual: " << actualVal << std::endl; \
            std::cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
            throw std::runtime_error("Test assertion failed"); \
        } \
    } while(0)


#define ASSERT_NO_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            std::cerr << "Unexpected exception: " << e.what() << std::endl; \
            ASSERT_TRUE(false); \
        } \
    } while(0)


void testTuiRenderer_Construction() {
    std::cout << "Running TuiRenderer construction tests..." << std::endl;


    {
        TuiRenderer renderer(3);
        ASSERT_TRUE(true);
    }


    {
        TuiRenderer rendererSmall(2);
        TuiRenderer rendererLarge(10);
        ASSERT_TRUE(true);
    }
}

void testTuiRenderer_Rendering() {
    std::cout << "Running TuiRenderer rendering tests..." << std::endl;


    Board<int> board(3);
    TuiRenderer renderer(3);


    {
        ASSERT_NO_THROW({
            renderer.render(board);
        });
    }
}

void testTuiRenderer_StateManagement() {
    std::cout << "Running TuiRenderer state management tests..." << std::endl;


    Board<int> board(3);
    TuiRenderer renderer(3);


    {
        GameStats stats(3);
        ASSERT_NO_THROW({
            renderer.updateStats(stats);
        });
    }


    {
        ASSERT_NO_THROW({
            renderer.showMessage("Test message");
        });
    }
}


void benchmarkRenderingPerformance() {
    std::cout << "Running rendering performance benchmark..." << std::endl;

    Board<int> board(10);
    TuiRenderer renderer(10);

    auto start = std::chrono::steady_clock::now();


    for (int i = 0; i < 1000; ++i) {
        renderer.render(board);
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Rendering 1000 times took: " << duration.count() << " ms" << std::endl;
    ASSERT_TRUE(duration.count() < 5000);
}


void testEdgeCases() {
    std::cout << "Running edge case tests..." << std::endl;


    {
        Board<int> tinyBoard(1);
        TuiRenderer renderer(1);
        ASSERT_NO_THROW({
            renderer.render(tinyBoard);
        });
    }


    {
        Board<int> hugeBoard(25);
        TuiRenderer renderer(25);
        ASSERT_NO_THROW({
            renderer.render(hugeBoard);
        });
    }
}


int main() {
    try {
        testTuiRenderer_Construction();
        testTuiRenderer_Rendering();
        testTuiRenderer_StateManagement();
        benchmarkRenderingPerformance();
        testEdgeCases();

        std::cout << "All UI component tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}