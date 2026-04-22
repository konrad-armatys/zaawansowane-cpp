#pragma once

#include <ftxui/dom/elements.hpp>

/**
 * @brief Renders the controls help bar
 */
class ControlsPanelRenderer {
public:
    /**
     * @brief Konstruktor domyślny
     */
    ControlsPanelRenderer() = default;

    /**
     * @brief Tworzy panel pomocy z informacją o klawiszach
     * @return Element FTXUI
     */
    ftxui::Element render() const;
};
