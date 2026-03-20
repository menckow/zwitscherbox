#pragma once

#include <Arduino.h>

// Konfiguration wird von /config.txt auf der SD-Karte geladen.
// Format: KEY=VALUE, Kommentarzeilen mit '#'
struct ZwitscherboxConfig {
    // MQTT Sync
    bool mqttIntegration = false;
    String mqttServer = "test.mosquitto.org";
    int mqttPort = 1883;
    String mqttUser = "";
    String mqttPass = "";
    String mqttClientIdBase = "esp32_zwitscherbox";
    String mqttSharedPlayTopic = "zwitscherbox/group/loveables/play";

    // WLAN
    String wifiSsid = "";
    String wifiPass = "";
};

class SdConfigManager {
public:
    SdConfigManager() = default;

    // Lädt config von der SD-Karte. Wenn nicht vorhanden, schreibt Default-Datei und lädt diese.
    bool load(const char* path = "/config.txt");

    // Schreibt eine config.txt mit aktuellen Werten auf die SD-Karte.
    bool save(const char* path = "/config.txt") const;

    const ZwitscherboxConfig& get() const { return _cfg; }
    ZwitscherboxConfig& getMutable() { return _cfg; }

private:
    ZwitscherboxConfig _cfg;

    static bool parseBool01(const String& v);
    void applyKeyValue(const String& key, const String& value);
    void setDefaults();
};

