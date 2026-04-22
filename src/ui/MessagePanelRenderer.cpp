#include "ui/MessagePanelRenderer.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Element MessagePanelRenderer::renderMessage(const std::string& message) const {
    if (message.empty()) {
        return text("");
    }

    return text(message) |
           bgcolor(Color::Blue) |
           color(Color::White) |
           bold;
}

ftxui::Element MessagePanelRenderer::renderInput(InputMode mode, const std::string& buffer) const {
    if (mode == InputMode::None) {
        return text("");
    }

    std::string modeLabel = (mode == InputMode::Save) ? "Zapisz:" : "Wczytaj:";

    return hbox({
        text(modeLabel) | bold,
        text(" [") | dim,
        text(buffer) | bgcolor(Color::White) | color(Color::Black),
        text("]") | dim
    });
}
