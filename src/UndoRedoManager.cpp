#include "UndoRedoManager.h"
#include <algorithm>

UndoRedoManager::UndoRedoManager(size_t maxHistorySize)
    : maxHistorySize_(maxHistorySize) {}

void UndoRedoManager::limitStackSize(std::stack<MoveData>& stack) {
    if (maxHistorySize_ == 0) {
        return;
    }


    if (stack.size() > maxHistorySize_) {
        std::stack<MoveData> tempStack;


        const size_t toKeep = std::min(maxHistorySize_, stack.size());

        for (size_t i = 0; i < toKeep; ++i) {
            tempStack.push(stack.top());
            stack.pop();
        }


        while (!stack.empty()) {
            stack.pop();
        }


        while (!tempStack.empty()) {
            stack.push(tempStack.top());
            tempStack.pop();
        }
    }
}

void UndoRedoManager::pushMove(int fromX, int fromY, int toX, int toY) {

    undoStack_.emplace(fromX, fromY, toX, toY);
    limitStackSize(undoStack_);



    clearRedoStack();
}

std::optional<MoveData> UndoRedoManager::undo() {
    if (!canUndo()) {
        return std::nullopt;
    }


    MoveData move = undoStack_.top();
    undoStack_.pop();


    redoStack_.push(move);
    limitStackSize(redoStack_);


    return move.reverse();
}

std::optional<MoveData> UndoRedoManager::redo() {
    if (!canRedo()) {
        return std::nullopt;
    }


    MoveData move = redoStack_.top();
    redoStack_.pop();


    undoStack_.push(move);
    limitStackSize(undoStack_);


    return move;
}

bool UndoRedoManager::canUndo() const noexcept {
    return !undoStack_.empty();
}

bool UndoRedoManager::canRedo() const noexcept {
    return !redoStack_.empty();
}

void UndoRedoManager::clear() {
    while (!undoStack_.empty()) {
        undoStack_.pop();
    }
    clearRedoStack();
}

void UndoRedoManager::clearRedoStack() {
    while (!redoStack_.empty()) {
        redoStack_.pop();
    }
}

size_t UndoRedoManager::getUndoStackSize() const noexcept {
    return undoStack_.size();
}

size_t UndoRedoManager::getRedoStackSize() const noexcept {
    return redoStack_.size();
}

void UndoRedoManager::setMaxHistorySize(size_t maxSize) {
    maxHistorySize_ = maxSize;
    limitStackSize(undoStack_);
    limitStackSize(redoStack_);
}

std::vector<MoveData> UndoRedoManager::exportUndoStack() const {
    std::vector<MoveData> result;
    std::stack<MoveData> tempStack = undoStack_;


    std::vector<MoveData> reversed;
    while (!tempStack.empty()) {
        reversed.push_back(tempStack.top());
        tempStack.pop();
    }

    std::reverse(reversed.begin(), reversed.end());
    return reversed;
}

std::vector<MoveData> UndoRedoManager::exportRedoStack() const {
    std::vector<MoveData> result;
    std::stack<MoveData> tempStack = redoStack_;

    std::vector<MoveData> reversed;
    while (!tempStack.empty()) {
        reversed.push_back(tempStack.top());
        tempStack.pop();
    }

    std::reverse(reversed.begin(), reversed.end());
    return reversed;
}

void UndoRedoManager::importUndoStack(const std::vector<MoveData>& moves) {

    while (!undoStack_.empty()) {
        undoStack_.pop();
    }


    for (const auto& move : moves) {
        undoStack_.push(move);
    }
}

void UndoRedoManager::importRedoStack(const std::vector<MoveData>& moves) {

    while (!redoStack_.empty()) {
        redoStack_.pop();
    }


    for (const auto& move : moves) {
        redoStack_.push(move);
    }
}
