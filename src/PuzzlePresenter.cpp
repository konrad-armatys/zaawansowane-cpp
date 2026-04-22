#include "PuzzlePresenter.h"
#include "TextGameSaver.h"
#include "ManhattanDistance.h"
#include "MoveAdvisor.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

PuzzlePresenter::PuzzlePresenter(int size,
                                 std::unique_ptr<IGameSaver> saver,
                                 std::unique_ptr<IHeuristic> heuristic,
                                 std::string savePath)
    : engine_(size),
      undoRedoManager_(100),
      saveHandler_(saver ? std::move(saver) : std::make_unique<TextGameSaver>()),
      heuristic_(heuristic ? std::move(heuristic) : std::make_unique<ManhattanDistance>()),
      inputMode_(InputMode::None),
      inputBuffer_(""),
      statusMessage_("Witaj w N-Puzzle! Użyj strzałek lub WSAD do gry"),
      savePath_(std::move(savePath)),
      hintPosition_(std::nullopt),
      onChange_() {
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

void PuzzlePresenter::notifyChanged() {
    if (onChange_) {
        onChange_();
    }
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
    hintPosition_ = std::nullopt;

    auto [emptyX, emptyY] = engine_.getEmptyPosition();

    bool success = engine_.move(dir, 0);

    if (success) {
        auto [newEmptyX, newEmptyY] = engine_.getEmptyPosition();
        undoRedoManager_.pushMove(emptyX, emptyY, newEmptyX, newEmptyY);

        if (engine_.isSolved()) {
            statusMessage_ = "Gratulacje! Układanka rozwiązana!";
        } else {
            statusMessage_ = "Ruch wykonany";
        }
    } else {
        statusMessage_ = "Nieprawidłowy ruch";
    }

    notifyChanged();
    return success;
}

bool PuzzlePresenter::undo() {
    hintPosition_ = std::nullopt;

    auto moveOpt = undoRedoManager_.undo();

    if (!moveOpt) {
        statusMessage_ = "Brak ruchów do cofnięcia";
        notifyChanged();
        return false;
    }

    const auto& move = *moveOpt;
    engine_.swapTiles(move.fromX, move.fromY, move.toX, move.toY);

    engine_.updateEmptyPosition(0);
    engine_.getStats().undoCount.set(engine_.getStats().undoCount.get() + 1);
    engine_.getStats().update(engine_.getBoard());

    statusMessage_ = "Cofnięto ruch";
    notifyChanged();
    return true;
}

bool PuzzlePresenter::redo() {
    hintPosition_ = std::nullopt;

    auto moveOpt = undoRedoManager_.redo();

    if (!moveOpt) {
        statusMessage_ = "Brak ruchów do ponowienia";
        notifyChanged();
        return false;
    }

    const auto& move = *moveOpt;
    engine_.swapTiles(move.fromX, move.fromY, move.toX, move.toY);

    engine_.updateEmptyPosition(0);
    int currentUndoCount = engine_.getStats().undoCount.get();
    if (currentUndoCount > 0) {
        engine_.getStats().undoCount.set(currentUndoCount - 1);
    }
    engine_.getStats().update(engine_.getBoard());

    statusMessage_ = "Ponowiono ruch";
    notifyChanged();
    return true;
}

void PuzzlePresenter::shuffle() {
    hintPosition_ = std::nullopt;
    engine_.shuffle(0);
    undoRedoManager_.clear();
    statusMessage_ = "Plansza przetasowana";
    notifyChanged();
}

void PuzzlePresenter::reset() {
    hintPosition_ = std::nullopt;
    engine_.reset(0);
    undoRedoManager_.clear();
    statusMessage_ = "Gra zresetowana";
    notifyChanged();
}

void PuzzlePresenter::hint(const MoveAdvisor& advisor) {
    auto [emptyX, emptyY] = engine_.getEmptyPosition();
    auto suggestion = advisor.suggestMove(engine_.getBoard(), emptyX, emptyY);

    if (!suggestion.has_value()) {
        hintPosition_ = std::nullopt;
        statusMessage_ = "Brak dostępnych podpowiedzi";
        notifyChanged();
        return;
    }

    const char* dirName = "";
    int highlightX = emptyX;
    int highlightY = emptyY;

    switch (*suggestion) {
        case Direction::Up:
            dirName = "Góra";
            highlightY = emptyY - 1;
            break;
        case Direction::Down:
            dirName = "Dół";
            highlightY = emptyY + 1;
            break;
        case Direction::Left:
            dirName = "Lewo";
            highlightX = emptyX - 1;
            break;
        case Direction::Right:
            dirName = "Prawo";
            highlightX = emptyX + 1;
            break;
    }

    hintPosition_ = std::make_pair(highlightX, highlightY);
    statusMessage_ = std::string("Podpowiedź: ") + dirName + " ⭐";
    notifyChanged();
}

void PuzzlePresenter::beginSaveMode() {
    inputMode_ = InputMode::Save;
    inputBuffer_ = savePath_;
    statusMessage_ = "Podaj ścieżkę do pliku zapisu (ESC - anuluj, ENTER - zapisz):";
    notifyChanged();
}

void PuzzlePresenter::beginLoadMode() {
    inputMode_ = InputMode::Load;
    inputBuffer_ = savePath_;
    statusMessage_ = "Podaj ścieżkę do pliku wczytania (ESC - anuluj, ENTER - wczytaj):";
    notifyChanged();
}

void PuzzlePresenter::confirmInputMode() {
    if (inputBuffer_.empty()) {
        statusMessage_ = "Błąd: ścieżka nie może być pusta";
        inputMode_ = InputMode::None;
        notifyChanged();
        return;
    }

    std::string resolvedPath = normalizePath(inputBuffer_);

    try {
        if (inputMode_ == InputMode::Save) {
            try {
                auto parentPath = std::filesystem::path(resolvedPath).parent_path();
                if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
                    std::filesystem::create_directories(parentPath);
                }
            } catch (...) {

            }

            saveGame(resolvedPath);
            savePath_ = resolvedPath;
            statusMessage_ = "Gra zapisana do " + resolvedPath;
        } else if (inputMode_ == InputMode::Load) {
            loadGame(resolvedPath);
            savePath_ = resolvedPath;
            statusMessage_ = "Gra wczytana z " + resolvedPath;
        }
    } catch (const std::exception& e) {
        statusMessage_ = std::string("Błąd: ") + e.what();
    }

    inputMode_ = InputMode::None;
    notifyChanged();
}

void PuzzlePresenter::cancelInputMode() {
    inputMode_ = InputMode::None;
    inputBuffer_.clear();
    statusMessage_ = "Anulowano";
    notifyChanged();
}

void PuzzlePresenter::handleCharacter(char c) {
    if (inputMode_ == InputMode::None) {
        return;
    }
    inputBuffer_ += c;
    notifyChanged();
}

void PuzzlePresenter::inputBackspace() {
    if (inputMode_ == InputMode::None) {
        return;
    }
    if (!inputBuffer_.empty()) {
        inputBuffer_.pop_back();
        notifyChanged();
    }
}

bool PuzzlePresenter::isInInputMode() const noexcept {
    return inputMode_ != InputMode::None;
}

InputMode PuzzlePresenter::getInputMode() const noexcept {
    return inputMode_;
}

const std::string& PuzzlePresenter::getInputBuffer() const noexcept {
    return inputBuffer_;
}

const std::string& PuzzlePresenter::getStatusMessage() const noexcept {
    return statusMessage_;
}

void PuzzlePresenter::setStatusMessage(std::string message) {
    statusMessage_ = std::move(message);
    notifyChanged();
}

const std::string& PuzzlePresenter::getSavePath() const noexcept {
    return savePath_;
}

void PuzzlePresenter::setSavePath(std::string path) {
    savePath_ = std::move(path);
}

void PuzzlePresenter::setHeuristic(std::unique_ptr<IHeuristic> heuristic) {
    heuristic_ = std::move(heuristic);
}

double PuzzlePresenter::getHeuristicValue() const {
    if (!heuristic_) {
        return 0.0;
    }
    return heuristic_->calculate(engine_.getBoard());
}

std::optional<std::pair<int, int>> PuzzlePresenter::getHintPosition() const noexcept {
    return hintPosition_;
}

void PuzzlePresenter::clearHintHighlight() {
    hintPosition_ = std::nullopt;
}

void PuzzlePresenter::setOnChange(std::function<void()> callback) {
    onChange_ = std::move(callback);

    if (onChange_) {
        auto& stats = engine_.getStats();
        stats.movesCount.setOnChange([cb = onChange_](int) { cb(); });
        stats.undoCount.setOnChange([cb = onChange_](int) { cb(); });
        stats.correctTiles.setOnChange([cb = onChange_](int) { cb(); });
    }
}

void PuzzlePresenter::bootstrap(bool autoLoadSave) {
    if (autoLoadSave) {
        try {
            loadGame(savePath_);
            statusMessage_ = "Gra wczytana z " + savePath_;
        } catch (...) {
            shuffle();
        }
    } else {
        shuffle();
    }
    notifyChanged();
}

PuzzleViewState PuzzlePresenter::getViewState() const {
    return PuzzleViewState{
        engine_.getBoard(),
        engine_.getStats(),
        statusMessage_,
        inputMode_,
        inputBuffer_,
        getHeuristicValue(),
        hintPosition_
    };
}

std::string PuzzlePresenter::normalizePath(const std::string& path) {
    std::string result = path;

    if (!result.empty() && result[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            result = std::string(home) + result.substr(1);
        }
    }

    try {
        if (std::filesystem::path(result).is_absolute()) {
            return result;
        }
    } catch (...) {

    }

    std::ifstream test(result);
    if (test.good()) {
        test.close();
        return result;
    }

    std::string parentPath = "../" + result;
    std::ifstream parentTest(parentPath);
    if (parentTest.good()) {
        parentTest.close();
        return parentPath;
    }

    return result;
}

std::string PuzzlePresenter::findSaveFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.good()) {
        file.close();
        return filename;
    }

    std::string parentPath = "../" + filename;
    std::ifstream parentFile(parentPath);
    if (parentFile.good()) {
        parentFile.close();
        return parentPath;
    }

    return "";
}

std::optional<std::string> PuzzlePresenter::detectSaveFile(const std::string& filename) {
    std::string found = findSaveFile(filename);
    if (found.empty()) {
        return std::nullopt;
    }
    return found;
}

std::optional<int> PuzzlePresenter::probeSavedBoardSize(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        return std::nullopt;
    }

    std::string firstLine;
    if (!std::getline(file, firstLine)) {
        return std::nullopt;
    }

    try {
        int value = std::stoi(firstLine);
        if (value <= 0) {
            return std::nullopt;
        }
        return value;
    } catch (...) {
        return std::nullopt;
    }
}
