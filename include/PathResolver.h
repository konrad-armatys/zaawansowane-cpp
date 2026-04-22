#pragma once
#include <optional>
#include <string>

/**
 * @brief Narzędzie do wyszukiwania i normalizacji ścieżek plików zapisu
 */
class PathResolver {
public:
    /**
     * @brief Normalizuje ścieżkę - rozszerza ~ na katalog domowy i obsługuje ścieżki absolutne
     * @param path Ścieżka do normalizacji
     * @return Znormalizowana ścieżka
     */
    static std::string normalizePath(const std::string& path);

    /**
     * @brief Sprawdza czy plik istnieje w katalogu bieżącym lub nadrzędnym
     * @param filename Nazwa pliku do sprawdzenia
     * @return Ścieżka do pliku jeśli istnieje, pusty string w przeciwnym wypadku
     */
    static std::string findSaveFile(const std::string& filename);

    /**
     * @brief Wykrywa istniejący plik zapisu w bieżącym lub nadrzędnym katalogu
     * @param filename Nazwa poszukiwanego pliku (np. "puzzle_save.txt")
     * @return std::optional ze ścieżką do pliku lub std::nullopt gdy plik nie istnieje
     */
    static std::optional<std::string> detectSaveFile(const std::string& filename);

    /**
     * @brief Odczytuje rozmiar planszy zapisanej w pierwszej linii pliku zapisu
     * @param filename Ścieżka do pliku zapisu
     * @return std::optional z rozmiarem planszy lub std::nullopt gdy odczyt się nie powiódł
     */
    static std::optional<int> probeSavedBoardSize(const std::string& filename);
};
