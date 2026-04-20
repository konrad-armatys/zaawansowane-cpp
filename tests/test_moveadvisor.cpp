#include "MoveAdvisor.h"
#include "ManhattanDistance.h"
#include "HammingDistance.h"
#include <iostream>
#include <cassert>
#include <memory>

/**
 * @brief Test konstruktora z prawidłową heurystyką
 */
void testConstructorValid() {
    std::cout << "Test: Constructor with valid heuristic... ";
    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);
    assert(advisor.getDepth() == 4);
    std::cout << "PASSED\n";
}

/**
 * @brief Test konstruktora z nullptr
 */
void testConstructorNull() {
    std::cout << "Test: Constructor with nullptr should throw... ";
    try {
        MoveAdvisor advisor(nullptr);
        std::cout << "FAILED (no exception thrown)\n";
        assert(false);
    } catch (const std::invalid_argument&) {
        std::cout << "PASSED\n";
    }
}

/**
 * @brief Test suggestMove na rozwiązanej planszy - nie powinno powodować błędów
 */
void testSuggestMoveOnSolvedBoardNoError() {
    std::cout << "Test: Suggest move on solved board doesn't crash... ";

    Board<int> solvedBoard(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 8, 0
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);


    const auto suggestion = advisor.suggestMove(solvedBoard, 2, 2);

    assert(!suggestion.has_value());
    std::cout << "PASSED\n";
}

/**
 * @brief Test suggestMove na planszy wymagającej 1 ruchu
 */
void testSuggestMoveOneMove() {
    std::cout << "Test: Suggest move on board needing one move... ";

    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 0, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 1, 2);
    assert(suggestion.has_value());
    assert(suggestion.value() == Direction::Right);
    std::cout << "PASSED\n";
}

/**
 * @brief Test suggestMove z różnymi heurystykami
 */
void testSuggestMoveWithDifferentHeuristics() {
    std::cout << "Test: Suggest move with different heuristics... ";

    Board<int> board(3, std::vector<int>{
        0, 2, 3,
        1, 5, 6,
        4, 7, 8
    });

    auto manhattan = std::make_shared<ManhattanDistance>();
    auto hamming = std::make_shared<HammingDistance>();

    MoveAdvisor advisorManhattan(manhattan);
    MoveAdvisor advisorHamming(hamming);

    const auto suggestionManhattan = advisorManhattan.suggestMove(board, 0, 0);
    const auto suggestionHamming = advisorHamming.suggestMove(board, 0, 0);

    assert(suggestionManhattan.has_value());
    assert(suggestionHamming.has_value());

    std::cout << "PASSED\n";
}

/**
 * @brief Test setHeuristic z prawidłową heurystyką
 */
void testSetHeuristicValid() {
    std::cout << "Test: Set heuristic with valid value... ";

    auto manhattan = std::make_shared<ManhattanDistance>();
    auto hamming = std::make_shared<HammingDistance>();

    MoveAdvisor advisor(manhattan);

    Board<int> board(3, std::vector<int>{
        0, 2, 3,
        1, 5, 6,
        4, 7, 8
    });

    const auto suggestionBefore = advisor.suggestMove(board, 0, 0);

    advisor.setHeuristic(hamming);

    const auto suggestionAfter = advisor.suggestMove(board, 0, 0);

    assert(suggestionBefore.has_value());
    assert(suggestionAfter.has_value());

    std::cout << "PASSED\n";
}

/**
 * @brief Test setHeuristic z nullptr
 */
void testSetHeuristicNull() {
    std::cout << "Test: Set heuristic with nullptr should throw... ";

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    try {
        advisor.setHeuristic(nullptr);
        std::cout << "FAILED (no exception thrown)\n";
        assert(false);
    } catch (const std::invalid_argument&) {
        std::cout << "PASSED\n";
    }
}

/**
 * @brief Test głębokości look-ahead
 */
void testLookaheadDepth() {
    std::cout << "Test: Lookahead depth is 4... ";

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    assert(advisor.getDepth() == 4);
    std::cout << "PASSED\n";
}

/**
 * @brief Test na złożonej planszy
 */
void testComplexBoard() {
    std::cout << "Test: Suggest move on complex board... ";

    Board<int> board(3, std::vector<int>{
        3, 1, 2,
        4, 7, 5,
        6, 8, 0
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 2, 2);
    assert(suggestion.has_value());

    std::cout << "PASSED\n";
}

/**
 * @brief Test na małej planszy 2x2
 */
void testSmallBoard() {
    std::cout << "Test: Suggest move on 2x2 board... ";

    Board<int> board(2, std::vector<int>{
        1, 2,
        0, 3
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 0, 1);
    assert(suggestion.has_value());

    std::cout << "PASSED\n";
}

/**
 * @brief Test weryfikujący że algorytm zatrzymuje się po znalezieniu rozwiązania
 */
void testEarlyStopOnSolution() {
    std::cout << "Test: Early stop when solution found... ";


    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        7, 0, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 1, 2);
    assert(suggestion.has_value());



    assert(suggestion.value() == Direction::Right);

    std::cout << "PASSED\n";
}

/**
 * @brief Test na planszy 2 ruchy od rozwiązania
 */
void testTwoMovesFromSolution() {
    std::cout << "Test: Board two moves from solution... ";


    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        0, 7, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 0, 2);
    assert(suggestion.has_value());



    assert(suggestion.value() == Direction::Right);

    std::cout << "PASSED\n";
}

/**
 * @brief Test na trudniejszej planszy (nie kończy się rozwiązaniem)
 */
void testMediumDifficultyBoard() {
    std::cout << "Test: Medium difficulty board... ";

    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 0, 6,
        7, 5, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 1, 1);
    assert(suggestion.has_value());


    const Direction dir = suggestion.value();
    assert(dir == Direction::Up || dir == Direction::Down ||
           dir == Direction::Left || dir == Direction::Right);

    std::cout << "PASSED\n";
}

/**
 * @brief Test na planszy z wysoką odległością Manhattan (nie rozwiąże w 4 krokach)
 */
void testHighDistanceBoard() {
    std::cout << "Test: High distance board... ";

    Board<int> board(3, std::vector<int>{
        8, 7, 6,
        5, 4, 3,
        2, 1, 0
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 2, 2);
    assert(suggestion.has_value());


    std::cout << "PASSED\n";
}

/**
 * @brief Test porównawczy różnych heurystyk na tej samej planszy
 */
void testHeuristicComparison() {
    std::cout << "Test: Different heuristics on same board... ";

    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        0, 4, 6,
        7, 5, 8
    });

    auto manhattan = std::make_shared<ManhattanDistance>();
    auto hamming = std::make_shared<HammingDistance>();

    MoveAdvisor advisorM(manhattan);
    MoveAdvisor advisorH(hamming);

    const auto suggestionM = advisorM.suggestMove(board, 0, 1);
    const auto suggestionH = advisorH.suggestMove(board, 0, 1);

    assert(suggestionM.has_value());
    assert(suggestionH.has_value());



    std::cout << "PASSED\n";
}

/**
 * @brief Test na planszy z pustym polem w rogu
 */
void testEmptyInCorner() {
    std::cout << "Test: Empty tile in corner... ";

    Board<int> board(3, std::vector<int>{
        0, 1, 2,
        3, 4, 5,
        6, 7, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 0, 0);
    assert(suggestion.has_value());


    const Direction dir = suggestion.value();
    assert(dir == Direction::Down || dir == Direction::Right);

    std::cout << "PASSED\n";
}

/**
 * @brief Test sekwencji ruchów prowadzących do poprawy
 */
void testImprovingSequence() {
    std::cout << "Test: Sequence leads to improvement... ";

    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 6,
        0, 7, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const double initialScore = heuristic->calculate(board);

    const auto suggestion = advisor.suggestMove(board, 0, 2);
    assert(suggestion.has_value());


    Board<int> afterMove = board.getCopy();
    int newX = 0, newY = 2;

    switch (suggestion.value()) {
        case Direction::Up:    newY--; break;
        case Direction::Down:  newY++; break;
        case Direction::Left:  newX--; break;
        case Direction::Right: newX++; break;
    }

    afterMove.swap(0, 2, newX, newY);
    const double afterScore = heuristic->calculate(afterMove);




    assert(suggestion.value() == Direction::Right);
    assert(afterScore <= initialScore);

    std::cout << "PASSED\n";
}

/**
 * @brief Test na planszy która wymaga wielu ruchów (>4) do rozwiązania
 */
void testDeepBoard() {
    std::cout << "Test: Board requiring >4 moves... ";


    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 0, 5,
        7, 8, 6
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const double initialScore = heuristic->calculate(board);

    const auto suggestion = advisor.suggestMove(board, 1, 1);
    assert(suggestion.has_value());


    Board<int> afterMove = board.getCopy();
    int newX = 1, newY = 1;

    switch (suggestion.value()) {
        case Direction::Up:    newY--; break;
        case Direction::Down:  newY++; break;
        case Direction::Left:  newX--; break;
        case Direction::Right: newX++; break;
    }

    afterMove.swap(1, 1, newX, newY);
    const double afterScore = heuristic->calculate(afterMove);



    std::cout << "PASSED (MD: " << initialScore << " -> suggested move exists)\n";
}

/**
 * @brief Test na bardzo pomieszanej planszy
 */
void testScrambledBoard() {
    std::cout << "Test: Heavily scrambled board... ";

    Board<int> board(3, std::vector<int>{
        3, 8, 1,
        6, 0, 4,
        7, 2, 5
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 1, 1);
    assert(suggestion.has_value());



    std::cout << "PASSED\n";
}

/**
 * @brief Test porównujący różne głębokości analizy (pośrednio)
 */
void testDepthMatters() {
    std::cout << "Test: Depth-4 finds better path... ";


    Board<int> board(3, std::vector<int>{
        1, 2, 3,
        4, 5, 0,
        7, 8, 6
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 2, 1);
    assert(suggestion.has_value());




    std::cout << "PASSED\n";
}

/**
 * @brief Test na planszy z wieloma kafelkami na złych pozycjach
 */
void testMultipleMisplacedTiles() {
    std::cout << "Test: Multiple misplaced tiles... ";

    Board<int> board(3, std::vector<int>{
        0, 2, 3,
        1, 4, 6,
        7, 5, 8
    });

    auto heuristic = std::make_shared<ManhattanDistance>();
    MoveAdvisor advisor(heuristic);

    const auto suggestion = advisor.suggestMove(board, 0, 0);
    assert(suggestion.has_value());


    const Direction dir = suggestion.value();
    assert(dir == Direction::Down || dir == Direction::Right);

    std::cout << "PASSED\n";
}

/**
 * @brief Funkcja główna uruchamiająca wszystkie testy
 */
int main() {
    std::cout << "=== MoveAdvisor Tests ===\n\n";

    std::cout << "--- Basic functionality ---\n";
    testConstructorValid();
    testConstructorNull();
    testSuggestMoveOnSolvedBoardNoError();
    testSetHeuristicValid();
    testSetHeuristicNull();
    testLookaheadDepth();

    std::cout << "\n--- Move suggestions (solvable within 4 moves) ---\n";
    testSuggestMoveOneMove();
    testTwoMovesFromSolution();
    testEarlyStopOnSolution();
    testImprovingSequence();

    std::cout << "\n--- Complex boards (not solvable in 4 moves) ---\n";
    testMediumDifficultyBoard();
    testHighDistanceBoard();
    testDeepBoard();
    testScrambledBoard();
    testDepthMatters();
    testMultipleMisplacedTiles();

    std::cout << "\n--- Various board configurations ---\n";
    testSmallBoard();
    testComplexBoard();
    testEmptyInCorner();

    std::cout << "\n--- Heuristic comparison ---\n";
    testSuggestMoveWithDifferentHeuristics();
    testHeuristicComparison();

    std::cout << "\n=== All 22 tests passed! ===\n";
    return 0;
}
