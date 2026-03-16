// ---------------------------------------------------------------- //
//                  DATEI: HardwareManager.cpp                      //
// ---------------------------------------------------------------- //
#include <Arduino.h>
#include "HardwareManager.h"
#include "config.h"

HardwareManager::HardwareManager(int potPin, int pirPin, int buttonPin) {
    _potPin = potPin;
    _pirPin = pirPin;
    _buttonPin = buttonPin;
}

void HardwareManager::begin() {
    pinMode(_potPin, INPUT);
    pinMode(_pirPin, INPUT);
    pinMode(_buttonPin, INPUT_PULLUP);
    _lastButtonState = digitalRead(_buttonPin);
}

void HardwareManager::update() {
    // 1. Knopf lesen und entprellen
    int reading = digitalRead(_buttonPin);
    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }
    if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (reading != _buttonState) {
            _buttonState = reading;
            if (_buttonState == LOW) {
                _buttonWasPressed = true; // Knopf wurde gedrückt
            }
        }
    }
    _lastButtonState = reading;

    // 2. Potentiometer lesen und glätten
    if (millis() - _lastPotReadTime >= POT_READ_INTERVAL_MS) {
        _lastPotReadTime = millis();
        long rawPotValue = analogRead(_potPin);
        if (_smoothedPotValue < 0) {
            _smoothedPotValue = rawPotValue << 8;
        } else {
            _smoothedPotValue = _smoothedPotValue + (rawPotValue << 8) - (_smoothedPotValue >> 3);
        }
        _lastVolume = map(_smoothedPotValue >> 8, 0, 4095, 0, 21);
    }
}

bool HardwareManager::isPirActive() {
    return digitalRead(_pirPin) == HIGH;
}

bool HardwareManager::wasButtonPressed() {
    if (_buttonWasPressed) {
        _buttonWasPressed = false; // Flag zurücksetzen, nachdem es abgefragt wurde
        return true;
    }
    return false;
}

int HardwareManager::getVolume() {
    return _lastVolume;
}
