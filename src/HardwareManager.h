#include <Arduino.h>

class HardwareManager {
public:
    HardwareManager(int potPin, int pirPin, int buttonPin);
    void begin();
    void update();

    bool isPirActive();
    bool wasButtonPressed(); // Gibt true nur einmal pro Klick zurück
    int getVolume();         // Gibt die geglättete Lautstärke von 0-21 zurück

private:
    // Pins
    int _potPin, _pirPin, _buttonPin;

    // Zustandsvariablen für den Knopf
    int _lastButtonState = HIGH;
    int _buttonState = HIGH;
    unsigned long _lastDebounceTime = 0;
    bool _buttonWasPressed = false;

    // Zustandsvariablen für das Potentiometer
    long _smoothedPotValue = -1;
    unsigned long _lastPotReadTime = 0;
    int _lastVolume = -1;
};