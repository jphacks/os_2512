#include "button_handler.h"

ButtonHandler::ButtonHandler() : lastButtonPressTime(0) {
}

void ButtonHandler::begin() {
    // M5StickCPlus2のボタン初期化は M5.begin() で行われるため、
    // 特別な初期化は不要
}

void ButtonHandler::update() {
    M5.update();
}

bool ButtonHandler::isButtonAPressed() {
    return M5.BtnA.wasPressed();
}

bool ButtonHandler::isButtonBPressed() {
    return M5.BtnB.wasPressed();
}

bool ButtonHandler::isDebounceTimeElapsed() {
    unsigned long currentTime = millis();
    return (currentTime - lastButtonPressTime) > DEBOUNCE_DELAY;
}

bool ButtonHandler::isButtonAPressedDebounced() {
    if (M5.BtnA.wasPressed() && isDebounceTimeElapsed()) {
        lastButtonPressTime = millis();
        return true;
    }
    return false;
}

bool ButtonHandler::isButtonBPressedDebounced() {
    if (M5.BtnB.wasPressed() && isDebounceTimeElapsed()) {
        lastButtonPressTime = millis();
        return true;
    }
    return false;
}