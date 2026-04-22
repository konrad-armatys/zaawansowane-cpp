#pragma once

#include "PuzzlePresenter.h"
#include <string>
#include <ftxui/dom/elements.hpp>

/**
 * @brief Renders status messages and input panels
 */
class MessagePanelRenderer {
public:
    /**
     * @brief Konstruktor domyślny
     */
    MessagePanelRenderer() = default;

    /**
     * @brief Tworzy panel komunikatu statusu
     * @param message Komunikat do wyświetlenia
     * @return Element FTXUI (pusty jeśli message jest puste)
     */
    ftxui::Element renderMessage(const std::string& message) const;

    /**
     * @brief Tworzy panel inputu dla zapisu/wczytywania
     * @param mode Tryb inputu (Save/Load/None)
     * @param buffer Zawartość bufora wprowadzania
     * @return Element FTXUI (pusty jeśli mode == None)
     */
    ftxui::Element renderInput(InputMode mode, const std::string& buffer) const;
};
