#pragma once // Verhindert, dass die Datei mehrfach eingebunden wird.

// --- Hardware Pin-Konfiguration ---
#define POT_PIN      4
#define PIR_PIN      18
#define BUTTON_PIN   17
#define SD_CS_PIN    SS

// I2S Pins für den Audio-Chip
#define I2S_BCLK_PIN     I2S_BCLK
#define I2S_LRCLK_PIN    I2S_LRCLK
#define I2S_DOUT_PIN     I2S_DOUT

// --- Timeout-Konfiguration (in Millisekunden) ---
#define MAX_PLAYBACK_DURATION   (5 * 60 * 1000UL)
#define DEEP_SLEEP_TIMEOUT      (5 * 60 * 1000UL)

// --- Andere Einstellungen ---
#define DEBOUNCE_DELAY_MS   50
#define POT_READ_INTERVAL_MS 100
