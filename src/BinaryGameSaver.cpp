#include "BinaryGameSaver.h"
#include <fstream>
#include <stdexcept>
#include <cstdint>

void BinaryGameSaver::validateFormat(uint32_t magic, uint32_t version) const {
    if (magic != MAGIC_NUMBER) {
        throw std::invalid_argument("Invalid file format: magic number mismatch. "
                                    "Expected NPUZ format.");
    }
    if (version > FORMAT_VERSION) {
        throw std::invalid_argument("Unsupported file version: " + std::to_string(version) +
                                    " (current version: " + std::to_string(FORMAT_VERSION) + ")");
    }
}

void BinaryGameSaver::writeMove(std::ostream& os, const MoveData& move) const {
    const int32_t fromX = static_cast<int32_t>(move.fromX);
    const int32_t fromY = static_cast<int32_t>(move.fromY);
    const int32_t toX = static_cast<int32_t>(move.toX);
    const int32_t toY = static_cast<int32_t>(move.toY);

    os.write(reinterpret_cast<const char*>(&fromX), sizeof(int32_t));
    os.write(reinterpret_cast<const char*>(&fromY), sizeof(int32_t));
    os.write(reinterpret_cast<const char*>(&toX), sizeof(int32_t));
    os.write(reinterpret_cast<const char*>(&toY), sizeof(int32_t));
}

MoveData BinaryGameSaver::readMove(std::istream& is) const {
    int32_t fromX, fromY, toX, toY;

    is.read(reinterpret_cast<char*>(&fromX), sizeof(int32_t));
    is.read(reinterpret_cast<char*>(&fromY), sizeof(int32_t));
    is.read(reinterpret_cast<char*>(&toX), sizeof(int32_t));
    is.read(reinterpret_cast<char*>(&toY), sizeof(int32_t));

    if (!is) {
        throw std::runtime_error("Failed to read move data from file");
    }

    return MoveData(fromX, fromY, toX, toY);
}

void BinaryGameSaver::save(const std::string& path, const GameState& state) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }


    file.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&FORMAT_VERSION), sizeof(uint32_t));


    const int32_t boardSize = static_cast<int32_t>(state.board.getSize());
    file.write(reinterpret_cast<const char*>(&boardSize), sizeof(int32_t));


    const auto& boardData = state.board.getData();
    for (const auto& value : boardData) {
        const int32_t val = static_cast<int32_t>(value);
        file.write(reinterpret_cast<const char*>(&val), sizeof(int32_t));
    }


    const int32_t movesCount = static_cast<int32_t>(state.stats.movesCount.get());
    const int32_t undoCount = static_cast<int32_t>(state.stats.undoCount.get());
    const int32_t correctTiles = static_cast<int32_t>(state.stats.correctTiles.get());

    file.write(reinterpret_cast<const char*>(&movesCount), sizeof(int32_t));
    file.write(reinterpret_cast<const char*>(&undoCount), sizeof(int32_t));
    file.write(reinterpret_cast<const char*>(&correctTiles), sizeof(int32_t));


    const uint32_t undoSize = static_cast<uint32_t>(state.undoHistory.size());
    file.write(reinterpret_cast<const char*>(&undoSize), sizeof(uint32_t));
    for (const auto& move : state.undoHistory) {
        writeMove(file, move);
    }


    const uint32_t redoSize = static_cast<uint32_t>(state.redoHistory.size());
    file.write(reinterpret_cast<const char*>(&redoSize), sizeof(uint32_t));
    for (const auto& move : state.redoHistory) {
        writeMove(file, move);
    }

    if (!file) {
        throw std::runtime_error("Failed to write game state to file: " + path);
    }
}

GameState BinaryGameSaver::load(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + path);
    }


    uint32_t magic, version;
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&version), sizeof(uint32_t));

    if (!file) {
        throw std::runtime_error("Failed to read file header: " + path);
    }

    validateFormat(magic, version);


    int32_t boardSize;
    file.read(reinterpret_cast<char*>(&boardSize), sizeof(int32_t));

    if (!file || boardSize <= 0) {
        throw std::invalid_argument("Invalid board size in file: " + std::to_string(boardSize));
    }


    std::vector<int> boardData;
    boardData.reserve(boardSize * boardSize);

    for (int i = 0; i < boardSize * boardSize; ++i) {
        int32_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
        if (!file) {
            throw std::runtime_error("Failed to read board data from file");
        }
        boardData.push_back(value);
    }


    int32_t movesCount, undoCount, correctTiles;
    file.read(reinterpret_cast<char*>(&movesCount), sizeof(int32_t));
    file.read(reinterpret_cast<char*>(&undoCount), sizeof(int32_t));
    file.read(reinterpret_cast<char*>(&correctTiles), sizeof(int32_t));

    if (!file) {
        throw std::runtime_error("Failed to read statistics from file");
    }


    uint32_t undoSize;
    file.read(reinterpret_cast<char*>(&undoSize), sizeof(uint32_t));

    if (!file) {
        throw std::runtime_error("Failed to read undo stack size from file");
    }

    std::vector<MoveData> undoHistory;
    undoHistory.reserve(undoSize);
    for (uint32_t i = 0; i < undoSize; ++i) {
        undoHistory.push_back(readMove(file));
    }


    uint32_t redoSize;
    file.read(reinterpret_cast<char*>(&redoSize), sizeof(uint32_t));

    if (!file) {
        throw std::runtime_error("Failed to read redo stack size from file");
    }

    std::vector<MoveData> redoHistory;
    redoHistory.reserve(redoSize);
    for (uint32_t i = 0; i < redoSize; ++i) {
        redoHistory.push_back(readMove(file));
    }


    Board<int> board(boardSize, std::move(boardData));
    GameStats stats(boardSize);

    stats.movesCount.set(movesCount);
    stats.undoCount.set(undoCount);
    stats.correctTiles.set(correctTiles);

    return GameState(std::move(board), std::move(stats),
                     std::move(undoHistory), std::move(redoHistory));
}
