#include "SdConfig.h"

#include <FS.h>
#include <SD.h>

static String trimCopy(String s) {
    s.trim();
    return s;
}

bool SdConfigManager::parseBool01(const String& v) {
    String t = v;
    t.trim();
    if (t == "1") return true;
    if (t == "true") return true;
    if (t == "TRUE") return true;
    if (t == "yes") return true;
    return false;
}

void SdConfigManager::setDefaults() {
    _cfg.mqttIntegration = false; // Default: Energiesparen, nur wenn explizit aktiviert
    _cfg.mqttServer = "test.mosquitto.org";
    _cfg.mqttPort = 1883;
    _cfg.mqttUser = "";
    _cfg.mqttPass = "";
    _cfg.mqttClientIdBase = "esp32_zwitscherbox";
    _cfg.mqttSharedPlayTopic = "zwitscherbox/group/loveables/play";

    _cfg.wifiSsid = "";
    _cfg.wifiPass = "";
}

void SdConfigManager::applyKeyValue(const String& key, const String& value) {
    if (key == "MQTT_INTEGRATION") _cfg.mqttIntegration = parseBool01(value);
    else if (key == "MQTT_SERVER") _cfg.mqttServer = value;
    else if (key == "MQTT_PORT") _cfg.mqttPort = value.toInt();
    else if (key == "MQTT_USER") _cfg.mqttUser = value;
    else if (key == "MQTT_PASS") _cfg.mqttPass = value;
    else if (key == "MQTT_CLIENT_ID_BASE") _cfg.mqttClientIdBase = value;
    else if (key == "MQTT_SHARED_PLAY_TOPIC") _cfg.mqttSharedPlayTopic = value;
    else if (key == "WIFI_SSID") _cfg.wifiSsid = value;
    else if (key == "WIFI_PASS") _cfg.wifiPass = value;
}

bool SdConfigManager::load(const char* path) {
    setDefaults();

    File f = SD.open(path, FILE_READ);
    if (!f) {
        Serial.println("No /config.txt on SD found. Writing defaults.");
        save(path);
        return true;
    }

    while (f.available()) {
        String line = f.readStringUntil('\n');
        line = trimCopy(line);
        if (line.length() == 0) continue;
        if (line[0] == '#') continue;

        int eq = line.indexOf('=');
        if (eq < 0) continue;

        String key = trimCopy(line.substring(0, eq));
        String value = trimCopy(line.substring(eq + 1));
        applyKeyValue(key, value);
    }

    f.close();

    Serial.print("Config loaded: MQTT_INTEGRATION=");
    Serial.println(_cfg.mqttIntegration ? "1" : "0");
    Serial.print("Config loaded: MQTT_SERVER=");
    Serial.println(_cfg.mqttServer);
    Serial.print("Config loaded: MQTT_SHARED_PLAY_TOPIC=");
    Serial.println(_cfg.mqttSharedPlayTopic);

    return true;
}

bool SdConfigManager::save(const char* path) const {
    SD.remove(path); // vermeidet Dopplungen/Append
    File f = SD.open(path, FILE_WRITE);
    if (!f) return false;

    // Minimal lesbarer Inhalt.
    f.println("# zwitscherbox /config.txt");
    f.println("# Format: KEY=VALUE (Kommentare: Zeilen beginnend mit '#')");
    f.println("# Beispiel: WIFI_SSID=mywifi");

    f.print("WIFI_SSID="); f.println(_cfg.wifiSsid);
    f.print("WIFI_PASS="); f.println(_cfg.wifiPass);
    f.println();

    f.print("MQTT_INTEGRATION="); f.println(_cfg.mqttIntegration ? "1" : "0");
    f.print("MQTT_SERVER="); f.println(_cfg.mqttServer);
    f.print("MQTT_PORT="); f.println(_cfg.mqttPort);
    f.print("MQTT_USER="); f.println(_cfg.mqttUser);
    f.print("MQTT_PASS="); f.println(_cfg.mqttPass);
    f.print("MQTT_CLIENT_ID_BASE="); f.println(_cfg.mqttClientIdBase);
    f.print("MQTT_SHARED_PLAY_TOPIC="); f.println(_cfg.mqttSharedPlayTopic);

    f.close();
    return true;
}

