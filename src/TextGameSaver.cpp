#include "TextGameSaver.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

void TextGameSaver::validateBoardSize(int size) const {
    if (size <= 0 || size > 1000) {
        throw std::invalid_argument("Invalid board size: " + std::to_string(size) +
                                    " (must be between 1 and 1000)");
    }
}

std::vector<int> TextGameSaver::parseIntegerLine(const std::string& line) const {
    std::istringstream ss(line);
    std::vector<int> result;
    int value;

    while (ss >> value) {
        result.push_back(value);
    }

    if (!ss.eof() && ss.fail()) {
        throw std::invalid_argument("Failed to parse integer from line: " + line);
    }

    return result;
}

MoveData TextGameSaver::parseMove(std::istringstream& ss) const {
    int fromX, fromY, toX, toY;

    if (!(ss >> fromX >> fromY >> toX >> toY)) {
        throw std::invalid_argument("Failed to parse move data");
    }

    return MoveData(fromX, fromY, toX, toY);
}

void TextGameSaver::save(const std::string& path, const GameState& state) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + path);
    }

    const int size = state.board.getSize();


    if (size > 100) {
        std::ios::sync_with_stdio(false);
    }

    try {

        file << size << '\n';


        const auto& data = state.board.getData();
        for (size_t i = 0; i < data.size(); ++i) {
            if (i > 0) file << ' ';
            file << data[i];
        }
        file << '\n';


        file << state.stats.movesCount.get() << ' '
             << state.stats.undoCount.get() << ' '
             << state.stats.correctTiles.get() << '\n';


        file << state.undoHistory.size() << ' ' << state.redoHistory.size();


        for (const auto& move : state.undoHistory) {
            file << ' ' << move.fromX << ' ' << move.fromY
                 << ' ' << move.toX << ' ' << move.toY;
        }


        for (const auto& move : state.redoHistory) {
            file << ' ' << move.fromX << ' ' << move.fromY
                 << ' ' << move.toX << ' ' << move.toY;
        }

        file << '\n';

        if (!file.good()) {
            throw std::runtime_error("Write operation failed");
        }

    } catch (const std::exception& e) {
        file.close();
        throw std::runtime_error("Failed to save game state: " + std::string(e.what()));
    }

    file.close();
}

GameState TextGameSaver::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + path);
    }

    try {
        std::string line;


        if (!std::getline(file, line)) {
            throw std::invalid_argument("File is empty or corrupted");
        }
        int size = std::stoi(line);
        validateBoardSize(size);


        if (!std::getline(file, line)) {
            throw std::invalid_argument("Missing board tiles data");
        }
        std::vector<int> tiles = parseIntegerLine(line);
        if (static_cast<int>(tiles.size()) != size * size) {
            throw std::invalid_argument("Board data size mismatch: expected " +
                                        std::to_string(size * size) + " tiles, got " +
                                        std::to_string(tiles.size()));
        }


        if (!std::getline(file, line)) {
            throw std::invalid_argument("Missing statistics data");
        }
        std::vector<int> stats = parseIntegerLine(line);
        if (stats.size() != 3) {
            throw std::invalid_argument("Invalid statistics format: expected 3 values, got " +
                                        std::to_string(stats.size()));
        }


        if (!std::getline(file, line)) {
            throw std::invalid_argument("Missing move history data");
        }
        std::istringstream historyStream(line);
        size_t undoStackSize, redoStackSize;

        if (!(historyStream >> undoStackSize >> redoStackSize)) {
            throw std::invalid_argument("Failed to parse stack sizes");
        }

        std::vector<MoveData> undoHistory;
        std::vector<MoveData> redoHistory;


        for (size_t i = 0; i < undoStackSize; ++i) {
            undoHistory.push_back(parseMove(historyStream));
        }


        for (size_t i = 0; i < redoStackSize; ++i) {
            redoHistory.push_back(parseMove(historyStream));
        }


        Board<int> board(size, std::move(tiles));
        GameStats gameStats(size);
        gameStats.movesCount.set(stats[0]);
        gameStats.undoCount.set(stats[1]);
        gameStats.correctTiles.set(stats[2]);

        file.close();

        return GameState(std::move(board), std::move(gameStats),
                         std::move(undoHistory), std::move(redoHistory));

    } catch (const std::invalid_argument& e) {
        file.close();
        throw std::invalid_argument("File is corrupted or has invalid format: " +
                                    std::string(e.what()));
    } catch (const std::exception& e) {
        file.close();
        throw std::runtime_error("Failed to load game state: " + std::string(e.what()));
    }
}
