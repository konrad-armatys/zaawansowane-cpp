#include "PathResolver.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>

std::string PathResolver::normalizePath(const std::string& path) {
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

std::string PathResolver::findSaveFile(const std::string& filename) {
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

std::optional<std::string> PathResolver::detectSaveFile(const std::string& filename) {
    std::string found = findSaveFile(filename);
    if (found.empty()) {
        return std::nullopt;
    }
    return found;
}

std::optional<int> PathResolver::probeSavedBoardSize(const std::string& filename) {
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
