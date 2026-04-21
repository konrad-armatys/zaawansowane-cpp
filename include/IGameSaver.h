#ifndef IGAMESAVER_H
#define IGAMESAVER_H

#include "GameState.h"
#include <string>
#include <memory>

/**
 * @brief Interfejs (klasa bazowa) definiujący kontrakt dla systemów zapisu/odczytu gry
 *
 * Ta klasa abstrakcyjna pozwala na implementację różnych strategii zapisu
 * (np. plik binarny, JSON, XML, baza danych) bez modyfikacji kodu klienta.
 * Wzorzec projektowy: Strategy/Template Method.
 */
class IGameSaver {
public:
    /**
     * @brief Wirtualny destruktor zapewniający poprawne czyszczenie obiektów pochodnych
     */
    virtual ~IGameSaver() = default;

    /**
     * @brief Zapisuje stan gry do pliku
     * @param path Ścieżka do pliku docelowego
     * @param state Stan gry do zapisania
     * @throws std::runtime_error Gdy zapis się nie powiedzie
     */
    virtual void save(const std::string& path, const GameState& state) = 0;

    /**
     * @brief Wczytuje stan gry z pliku
     * @param path Ścieżka do pliku źródłowego
     * @return Wczytany stan gry
     * @throws std::runtime_error Gdy odczyt się nie powiedzie
     * @throws std::invalid_argument Gdy plik ma nieprawidłowy format
     */
    virtual GameState load(const std::string& path) = 0;

protected:
    /**
     * @brief Konstruktor chroniony - zapobiega bezpośredniemu tworzeniu instancji
     */
    IGameSaver() = default;

    /**
     * @brief Konstruktor kopiujący chroniony
     */
    IGameSaver(const IGameSaver&) = default;

    /**
     * @brief Operator przypisania chroniony
     */
    IGameSaver& operator=(const IGameSaver&) = default;

    /**
     * @brief Konstruktor przenoszący chroniony
     */
    IGameSaver(IGameSaver&&) = default;

    /**
     * @brief Operator przenoszącego przypisania chroniony
     */
    IGameSaver& operator=(IGameSaver&&) = default;
};

#endif
