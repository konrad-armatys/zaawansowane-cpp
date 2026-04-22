/**
 * @file test_concepts.cpp
 * @brief Testy walidacji C++20 concepts dla N-Puzzle
 *
 * Demonstruje:
 * - Poprawne typy spełniające PuzzleTile
 * - Poprawne typy spełniające NumericTile
 * - Typy, które NIE spełniają wymagań (commented out, by się nie kompilowały)
 */

#include "gtest/gtest.h"
#include "PuzzleConcepts.h"
#include "Board.h"
#include <string>
#include <vector>





struct SimpleComparableTile {
    int value;

    auto operator<=>(const SimpleComparableTile&) const = default;
    bool operator==(const SimpleComparableTile&) const = default;
};

struct FullNumericTile {
    int value;

    auto operator<=>(const FullNumericTile&) const = default;
    bool operator==(const FullNumericTile&) const = default;

    int operator-(const FullNumericTile& other) const {
        return value - other.value;
    }

    explicit operator int() const { return value; }
};





TEST(ConceptsTest, IntSatisfiesPuzzleTile) {
    static_assert(PuzzleTile<int>);
    SUCCEED();
}

TEST(ConceptsTest, IntSatisfiesNumericTile) {
    static_assert(NumericTile<int>);
    SUCCEED();
}

TEST(ConceptsTest, LongSatisfiesPuzzleTile) {
    static_assert(PuzzleTile<long>);
    SUCCEED();
}

TEST(ConceptsTest, LongSatisfiesNumericTile) {
    static_assert(NumericTile<long>);
    SUCCEED();
}

TEST(ConceptsTest, SimpleComparableSatisfiesPuzzleTile) {
    static_assert(PuzzleTile<SimpleComparableTile>);
    SUCCEED();
}

TEST(ConceptsTest, FullNumericTileSatisfiesPuzzleTile) {
    static_assert(PuzzleTile<FullNumericTile>);
    SUCCEED();
}

TEST(ConceptsTest, FullNumericTileSatisfiesNumericTile) {
    static_assert(NumericTile<FullNumericTile>);
    SUCCEED();
}





TEST(BoardTemplateTest, CanInstantiateWithInt) {
    Board<int> board(3);
    EXPECT_EQ(board.getSize(), 3);
}

TEST(BoardTemplateTest, CanInstantiateWithLong) {
    Board<long> board(4);
    EXPECT_EQ(board.getSize(), 4);
}

TEST(BoardTemplateTest, CanInstantiateWithCustomTile) {
    Board<SimpleComparableTile> board(2);
    EXPECT_EQ(board.getSize(), 2);
}

TEST(BoardTemplateTest, CanInstantiateWithNumericTile) {
    Board<FullNumericTile> board(3);
    EXPECT_EQ(board.getSize(), 3);
}






#include "ManhattanDistance.h"
#include "HammingDistance.h"

TEST(HeuristicTemplateTest, CanInstantiateManhattanWithInt) {
    Board<int> board(3);
    ManhattanDistance<int> heuristic;
    EXPECT_NO_THROW({
        double score = heuristic.calculate(board);
        EXPECT_GE(score, 0.0);
    });
}

TEST(HeuristicTemplateTest, CanInstantiateManhattanWithLong) {
    Board<long> board(3);
    ManhattanDistance<long> heuristic;
    EXPECT_NO_THROW({
        double score = heuristic.calculate(board);
        EXPECT_GE(score, 0.0);
    });
}

TEST(HeuristicTemplateTest, CanInstantiateHammingWithInt) {
    Board<int> board(3);
    HammingDistance<int> heuristic;
    EXPECT_NO_THROW({
        double score = heuristic.calculate(board);
        EXPECT_GE(score, 0.0);
    });
}

TEST(HeuristicTemplateTest, CanInstantiateHammingWithSimpleComparable) {
    Board<SimpleComparableTile> board(3);
    HammingDistance<SimpleComparableTile> heuristic;
    EXPECT_NO_THROW({
        double score = heuristic.calculate(board);
        EXPECT_GE(score, 0.0);
    });
}






























TEST(TemplateInstantiationTest, MultipleSpecializations) {

    Board<int> b1(3);
    Board<long> b2(3);
    Board<SimpleComparableTile> b3(3);
    Board<FullNumericTile> b4(3);

    ManhattanDistance<int> m1;
    ManhattanDistance<long> m2;
    ManhattanDistance<FullNumericTile> m3;

    HammingDistance<int> h1;
    HammingDistance<SimpleComparableTile> h2;

    EXPECT_NO_THROW({
        m1.calculate(b1);
        m2.calculate(b2);
        m3.calculate(b4);
        h1.calculate(b1);
        h2.calculate(b3);
    });
}
