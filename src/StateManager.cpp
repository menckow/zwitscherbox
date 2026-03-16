#include <Arduino.h>
#include "StateManager.h"
#include "config.h"

StateManager::StateManager() {}

void StateManager::begin() {
    // Initialisiere Preferences im "appState" Namespace
    if (!preferences.begin("appState", false)) {
        Serial.println("Error starting Preferences. Using read-only mode.");
        preferences.begin("appState", true);
    }
}

void StateManager::saveState(int dirIndex, int volume) {
    preferences.putInt("dirIndex", dirIndex);
    preferences.putInt("volume", volume);
    Serial.printf("State saved: DirIndex=%d, Volume=%d\n", dirIndex, volume);
}

void StateManager::loadState(int &dirIndex, int &volume) {
    dirIndex = preferences.getInt("dirIndex", 0);
    volume = preferences.getInt("volume", 15);
    Serial.printf("State loaded: DirIndex=%d, Volume=%d\n", dirIndex, volume);
}

void StateManager::checkAndEnterDeepSleep(unsigned long lastActivityTime, int pirPin) {
    if (millis() - lastActivityTime >= DEEP_SLEEP_TIMEOUT) {
        Serial.println("--- Inactivity detected. Entering deep sleep. ---");
        Serial.flush();
        esp_sleep_enable_ext0_wakeup((gpio_num_t)pirPin, 1);
        esp_deep_sleep_start();
    }
}

