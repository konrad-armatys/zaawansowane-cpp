#include "UndoRedo.h"
#include <algorithm>

UndoRedo::UndoRedo(size_t maxHistorySize)
    : maxHistorySize_(maxHistorySize) {}

void UndoRedo::limitStackSize(std::stack<GameState>& stack) {
    if (maxHistorySize_ == 0) {
        return;
    }


    if (stack.size() > maxHistorySize_) {
        std::stack<GameState> tempStack;


        size_t toKeep = std::min(maxHistorySize_, stack.size());

        for (size_t i = 0; i < toKeep; ++i) {
            tempStack.push(std::move(stack.top()));
            stack.pop();
        }


        while (!stack.empty()) {
            stack.pop();
        }


        while (!tempStack.empty()) {
            stack.push(std::move(tempStack.top()));
            tempStack.pop();
        }
    }
}

void UndoRedo::pushState(const Board<int>& board, int emptyX, int emptyY, int moveCount) {
    undoStack_.emplace(board.getCopy(), emptyX, emptyY, moveCount);
    limitStackSize(undoStack_);


    clearRedoStack();
}

std::optional<GameState> UndoRedo::undo() {
    if (!canUndo()) {
        return std::nullopt;
    }


    GameState currentState = std::move(undoStack_.top());
    undoStack_.pop();

    redoStack_.push(std::move(currentState));
    limitStackSize(redoStack_);


    if (!undoStack_.empty()) {
        GameState previousState = undoStack_.top();
        return previousState;
    }

    return std::nullopt;
}

std::optional<GameState> UndoRedo::redo() {
    if (!canRedo()) {
        return std::nullopt;
    }


    GameState stateToRestore = std::move(redoStack_.top());
    redoStack_.pop();


    undoStack_.push(stateToRestore);

    return stateToRestore;
}

bool UndoRedo::canUndo() const noexcept {
    return !undoStack_.empty();
}

bool UndoRedo::canRedo() const noexcept {
    return !redoStack_.empty();
}

void UndoRedo::clear() {
    while (!undoStack_.empty()) {
        undoStack_.pop();
    }
    while (!redoStack_.empty()) {
        redoStack_.pop();
    }
}

void UndoRedo::clearRedoStack() {
    while (!redoStack_.empty()) {
        redoStack_.pop();
    }
}

size_t UndoRedo::getUndoStackSize() const noexcept {
    return undoStack_.size();
}

size_t UndoRedo::getRedoStackSize() const noexcept {
    return redoStack_.size();
}

void UndoRedo::setMaxHistorySize(size_t maxSize) {
    maxHistorySize_ = maxSize;
    limitStackSize(undoStack_);
    limitStackSize(redoStack_);
}
