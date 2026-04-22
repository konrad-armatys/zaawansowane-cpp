#include "InputModeHandler.h"

InputModeHandler::InputModeHandler()
    : inputMode_(InputMode::None), inputBuffer_("") {}

void InputModeHandler::beginSaveMode(const std::string& initialBuffer) {
    inputMode_ = InputMode::Save;
    inputBuffer_ = initialBuffer;
}

void InputModeHandler::beginLoadMode(const std::string& initialBuffer) {
    inputMode_ = InputMode::Load;
    inputBuffer_ = initialBuffer;
}

bool InputModeHandler::confirm() {
    inputMode_ = InputMode::None;
    return !inputBuffer_.empty();
}

void InputModeHandler::cancel() {
    inputMode_ = InputMode::None;
    inputBuffer_.clear();
}

void InputModeHandler::handleCharacter(char c) {
    if (inputMode_ == InputMode::None) {
        return;
    }
    inputBuffer_ += c;
}

void InputModeHandler::inputBackspace() {
    if (inputMode_ == InputMode::None) {
        return;
    }
    if (!inputBuffer_.empty()) {
        inputBuffer_.pop_back();
    }
}

bool InputModeHandler::isInInputMode() const noexcept {
    return inputMode_ != InputMode::None;
}

InputMode InputModeHandler::getInputMode() const noexcept {
    return inputMode_;
}

const std::string& InputModeHandler::getInputBuffer() const noexcept {
    return inputBuffer_;
}
