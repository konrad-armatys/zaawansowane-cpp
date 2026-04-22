#include "PuzzlePresenter.h"
#include "TextGameSaver.h"
#include <stdexcept>

PuzzlePresenter::PuzzlePresenter(int size, std::unique_ptr<IGameSaver> saver)
    : engine_(size),
      undoRedoManager_(100),
      saveHandler_(saver ? std::move(saver) : std::make_unique<TextGameSaver>()) {
}

GameState PuzzlePresenter::captureCurrentState() const {
    return GameState(
        engine_.getBoard(),
        engine_.getStats(),
        undoRedoManager_.exportUndoStack(),
        undoRedoManager_.exportRedoStack()
    );
}

void PuzzlePresenter::restoreState(const GameState& state) {
    engine_ = PuzzleEngine<int>(state.board, state.stats, 0);

    undoRedoManager_.clear();
    undoRedoManager_.importUndoStack(state.undoHistory);
    undoRedoManager_.importRedoStack(state.redoHistory);
}

void PuzzlePresenter::saveGame(const std::string& filename) {
    if (!saveHandler_) {
        throw std::runtime_error("Save handler not set - cannot save game");
    }

    GameState currentState = captureCurrentState();
    saveHandler_->save(filename, currentState);
}

void PuzzlePresenter::loadGame(const std::string& filename) {
    if (!saveHandler_) {
        throw std::runtime_error("Save handler not set - cannot load game");
    }

    GameState loadedState = saveHandler_->load(filename);
    restoreState(loadedState);
}

void PuzzlePresenter::setSaveHandler(std::unique_ptr<IGameSaver> saver) {
    if (!saver) {
        throw std::invalid_argument("Cannot set null save handler");
    }
    saveHandler_ = std::move(saver);
}

PuzzleEngine<int>& PuzzlePresenter::getEngine() noexcept {
    return engine_;
}

const PuzzleEngine<int>& PuzzlePresenter::getEngine() const noexcept {
    return engine_;
}

UndoRedoManager& PuzzlePresenter::getUndoRedoManager() noexcept {
    return undoRedoManager_;
}

const UndoRedoManager& PuzzlePresenter::getUndoRedoManager() const noexcept {
    return undoRedoManager_;
}

bool PuzzlePresenter::move(Direction dir) {
    auto [emptyX, emptyY] = engine_.getEmptyPosition();

    bool success = engine_.move(dir, 0);

    if (success) {
        auto [newEmptyX, newEmptyY] = engine_.getEmptyPosition();
        undoRedoManager_.pushMove(emptyX, emptyY, newEmptyX, newEmptyY);
    }

    return success;
}

bool PuzzlePresenter::undo() {
    auto moveOpt = undoRedoManager_.undo();

    if (!moveOpt) {
        return false;
    }

    auto& board = const_cast<Board<int>&>(engine_.getBoard());
    const auto& move = *moveOpt;

    board.swap(move.fromX, move.fromY, move.toX, move.toY);

    engine_.updateEmptyPosition(0);
    engine_.getStats().undoCount.set(engine_.getStats().undoCount.get() + 1);
    engine_.getStats().update(board);

    return true;
}

bool PuzzlePresenter::redo() {
    auto moveOpt = undoRedoManager_.redo();

    if (!moveOpt) {
        return false;
    }

    auto& board = const_cast<Board<int>&>(engine_.getBoard());
    const auto& move = *moveOpt;

    board.swap(move.fromX, move.fromY, move.toX, move.toY);

    engine_.updateEmptyPosition(0);
    int currentUndoCount = engine_.getStats().undoCount.get();
    if (currentUndoCount > 0) {
        engine_.getStats().undoCount.set(currentUndoCount - 1);
    }
    engine_.getStats().update(board);

    return true;
}

void PuzzlePresenter::shuffle() {
    engine_.shuffle(0);
    undoRedoManager_.clear();
}

void PuzzlePresenter::reset() {
    engine_.reset(0);
    undoRedoManager_.clear();
}
