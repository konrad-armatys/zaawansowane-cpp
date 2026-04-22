#pragma once
#include "InputMode.h"
#include <string>

/**
 * @brief Maszyna stanów zarządzająca trybem wpisywania ścieżki zapisu/odczytu
 *
 * Enkapsuluje inputMode_, inputBuffer_ oraz całą logikę przejść między stanami.
 * Nie wykonuje operacji zapisu ani odczytu — po zatwierdzeniu bufor i tryb są
 * dostępne dla prezentera przez gettery.
 */
class InputModeHandler {
private:
    InputMode inputMode_;
    std::string inputBuffer_;

public:
    /**
     * @brief Konstruktor ustawiający stan początkowy (None, pusty bufor)
     */
    InputModeHandler();

    /**
     * @brief Rozpoczyna tryb wpisywania ścieżki do zapisu
     * @param initialBuffer Początkowa wartość bufora (np. bieżąca ścieżka zapisu)
     */
    void beginSaveMode(const std::string& initialBuffer);

    /**
     * @brief Rozpoczyna tryb wpisywania ścieżki do odczytu
     * @param initialBuffer Początkowa wartość bufora (np. bieżąca ścieżka zapisu)
     */
    void beginLoadMode(const std::string& initialBuffer);

    /**
     * @brief Zatwierdza tryb — czyści inputMode_ i zwraca true jeśli bufor jest niepusty
     *
     * Po wywołaniu inputMode_ == None, a inputBuffer_ zachowuje ostatnią wartość
     * do odczytania przez prezenter przed kolejnym beginSave/LoadMode.
     *
     * @return true jeśli bufor był niepusty i można kontynuować operację
     */
    bool confirm();

    /**
     * @brief Anuluje tryb — czyści inputMode_ i inputBuffer_
     */
    void cancel();

    /**
     * @brief Dodaje znak do bufora (ignorowane gdy tryb == None)
     * @param c Znak do dodania
     */
    void handleCharacter(char c);

    /**
     * @brief Usuwa ostatni znak z bufora (ignorowane gdy tryb == None lub bufor pusty)
     */
    void inputBackspace();

    /**
     * @brief Sprawdza czy aktywny jest jakikolwiek tryb wpisywania
     * @return true jeśli inputMode_ != None
     */
    bool isInInputMode() const noexcept;

    /**
     * @brief Pobiera aktualny tryb wejścia
     * @return Aktualny InputMode
     */
    InputMode getInputMode() const noexcept;

    /**
     * @brief Pobiera aktualny bufor wejściowy
     * @return Stała referencja do bufora
     */
    const std::string& getInputBuffer() const noexcept;
};
