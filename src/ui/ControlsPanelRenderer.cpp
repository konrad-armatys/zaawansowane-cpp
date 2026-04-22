#include "ui/ControlsPanelRenderer.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Element ControlsPanelRenderer::render() const {
    return text("Sterowanie: ↑↓←→/WSAD - ruch | U - undo | R - redo | H - hint | V - save | L - load | N - new | Q - quit") |
           dim | center;
}
