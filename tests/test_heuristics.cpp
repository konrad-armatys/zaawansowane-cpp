#include "Board.h"
#include "ManhattanDistance.h"
#include "HammingDistance.h"
#include "LookAhead.h"
#include "UndoRedo.h"
#include <iostream>
#include <memory>
#include <cassert>

void testManhattanDistance() {
    std::cout << "Testing Manhattan Distance..." << std::endl;


    Board<int> solvedBoard(3, {1, 2, 3, 4, 5, 6, 7, 8, 0});
    ManhattanDistance<int> manhattan;

    double score = manhattan.calculate(solvedBoard);
    assert(score == 0.0 && "Solved board should have Manhattan distance of 0");
    std::cout << "  Solved board score: " << score << " (expected 0)" << std::endl;


    Board<int> oneWrongBoard(3, {1, 2, 3, 4, 5, 6, 7, 0, 8});
    score = manhattan.calculate(oneWrongBoard);
    std::cout << "  One wrong tile score: " << score << " (expected 2)" << std::endl;


    Board<int> randomBoard(3, {8, 1, 3, 4, 0, 2, 7, 6, 5});
    score = manhattan.calculate(randomBoard);
    std::cout << "  Random board score: " << score << std::endl;

    std::cout << "Manhattan Distance tests passed!" << std::endl << std::endl;
}

void testHammingDistance() {
    std::cout << "Testing Hamming Distance..." << std::endl;


    Board<int> solvedBoard(3, {1, 2, 3, 4, 5, 6, 7, 8, 0});
    HammingDistance<int> hamming;

    double score = hamming.calculate(solvedBoard);
    assert(score == 0.0 && "Solved board should have Hamming distance of 0");
    std::cout << "  Solved board score: " << score << " (expected 0)" << std::endl;


    Board<int> oneWrongBoard(3, {1, 2, 3, 4, 5, 6, 7, 0, 8});
    score = hamming.calculate(oneWrongBoard);
    std::cout << "  One wrong tile score: " << score << " (expected 1)" << std::endl;


    Board<int> randomBoard(3, {8, 1, 3, 4, 0, 2, 7, 6, 5});
    score = hamming.calculate(randomBoard);
    std::cout << "  Random board score: " << score << std::endl;

    std::cout << "Hamming Distance tests passed!" << std::endl << std::endl;
}

void testLookAhead() {
    std::cout << "Testing Look-ahead system..." << std::endl;


    Board<int> nearSolutionBoard(3, {1, 2, 3, 4, 5, 6, 7, 0, 8});

    auto heuristic = std::make_shared<ManhattanDistance<int>>();
    LookAhead<int> lookAhead(heuristic);


    int emptyX = 1, emptyY = 2;

    auto evaluations = lookAhead.evaluateMoves(nearSolutionBoard, emptyX, emptyY);

    std::cout << "  Possible moves evaluation:" << std::endl;
    for (const auto& eval : evaluations) {
        std::string dirName;
        switch (eval.direction) {
            case Direction::Up:    dirName = "Up";    break;
            case Direction::Down:  dirName = "Down";  break;
            case Direction::Left:  dirName = "Left";  break;
            case Direction::Right: dirName = "Right"; break;
        }
        std::cout << "    " << dirName << ": " << eval.score << std::endl;
    }

    auto bestMove = lookAhead.getBestMove(nearSolutionBoard, emptyX, emptyY);
    assert(bestMove.has_value() && "Should find at least one move");

    std::cout << "  Best move found!" << std::endl;


    auto hammingHeuristic = std::make_shared<HammingDistance<int>>();
    lookAhead.setHeuristic(hammingHeuristic);

    evaluations = lookAhead.evaluateMoves(nearSolutionBoard, emptyX, emptyY);
    std::cout << "  Evaluation with Hamming Distance:" << std::endl;
    for (const auto& eval : evaluations) {
        std::string dirName;
        switch (eval.direction) {
            case Direction::Up:    dirName = "Up";    break;
            case Direction::Down:  dirName = "Down";  break;
            case Direction::Left:  dirName = "Left";  break;
            case Direction::Right: dirName = "Right"; break;
        }
        std::cout << "    " << dirName << ": " << eval.score << std::endl;
    }

    std::cout << "Look-ahead tests passed!" << std::endl << std::endl;
}

void testUndoRedo() {
    std::cout << "Testing Undo/Redo system..." << std::endl;

    UndoRedo undoRedo(10);

    Board<int> board1(3, {1, 2, 3, 4, 5, 6, 7, 8, 0});
    Board<int> board2(3, {1, 2, 3, 4, 5, 6, 7, 0, 8});
    Board<int> board3(3, {1, 2, 3, 4, 0, 6, 7, 5, 8});


    undoRedo.pushState(board1, 2, 2, 0);
    assert(undoRedo.canUndo() && "Should be able to undo");
    assert(!undoRedo.canRedo() && "Should not be able to redo");
    std::cout << "  State 1 pushed. Undo size: " << undoRedo.getUndoStackSize() << std::endl;

    undoRedo.pushState(board2, 1, 2, 1);
    std::cout << "  State 2 pushed. Undo size: " << undoRedo.getUndoStackSize() << std::endl;

    undoRedo.pushState(board3, 1, 1, 2);
    std::cout << "  State 3 pushed. Undo size: " << undoRedo.getUndoStackSize() << std::endl;


    auto state = undoRedo.undo();
    assert(state.has_value() && "Undo should return a state");
    assert(state->board == board2 && "Should return to state 2");
    std::cout << "  Undo successful. Back to state 2" << std::endl;
    std::cout << "  Undo size: " << undoRedo.getUndoStackSize()
              << ", Redo size: " << undoRedo.getRedoStackSize() << std::endl;

    assert(undoRedo.canRedo() && "Should be able to redo");


    state = undoRedo.redo();
    assert(state.has_value() && "Redo should return a state");
    assert(state->board == board3 && "Should return to state 3");
    std::cout << "  Redo successful. Back to state 3" << std::endl;
    std::cout << "  Undo size: " << undoRedo.getUndoStackSize()
              << ", Redo size: " << undoRedo.getRedoStackSize() << std::endl;


    state = undoRedo.undo();
    Board<int> board4(3, {1, 2, 3, 0, 4, 6, 7, 5, 8});
    undoRedo.pushState(board4, 0, 1, 3);
    assert(!undoRedo.canRedo() && "Redo stack should be cleared after new move");
    std::cout << "  New move after undo cleared redo stack" << std::endl;


    undoRedo.clear();
    assert(!undoRedo.canUndo() && "Should not be able to undo after clear");
    assert(!undoRedo.canRedo() && "Should not be able to redo after clear");
    std::cout << "  Clear successful" << std::endl;

    std::cout << "Undo/Redo tests passed!" << std::endl << std::endl;
}

int main() {
    try {
        testManhattanDistance();
        testHammingDistance();
        testLookAhead();
        testUndoRedo();

        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
