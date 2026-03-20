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

// --- Netzwerk / MQTT Sync ---
// Hinweis: Wenn MQTT_INTEGRATION = 1, wird Deep Sleep in main.cpp deaktiviert,
// damit die Box weiterhin Nachrichten empfangen kann.
#define MQTT_INTEGRATION 1

// Öffentliche Broker Beispiel: "test.mosquitto.org"
#define MQTT_SERVER       "test.mosquitto.org"
#define MQTT_PORT         1883

// Für öffentliche Broker oft leer (keine Auth erforderlich).
#define MQTT_USER         ""
#define MQTT_PASS         ""

// Wird zur Erstellung einer eindeutigen Client-ID verwendet.
#define MQTT_CLIENT_ID_BASE "esp32_zwitscherbox"

// Gemeinsames Topic (Gruppen-Topic), auf dem alle Boxen Play-Events
// veröffentlichen/abhören. Gruppenname: "loveables"
#define MQTT_SHARED_PLAY_TOPIC "zwitscherbox/group/loveables/play"

// WLAN-Zugangsdaten (je Box identisch, wenn gleicher AP).
// Wenn leer, kann MQTT nicht verbinden.
#define WIFI_SSID ""
#define WIFI_PASS ""
