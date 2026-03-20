#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"

class PlaybackManager;

class MqttSyncManager {
public:
    explicit MqttSyncManager(PlaybackManager* player);

    void begin();
    void update();

    // Wird vom Player ausgelöst, wenn lokal erfolgreich eine MP3 gestartet wurde.
    void notifyLocalPlay(const String& dirPath, const String& fileName);

    bool isEnabled() const { return MQTT_INTEGRATION != 0; }

private:
    void ensureWifi();
    void ensureMqttConnected();
    void onMqttMessage(char* topic, uint8_t* payload, unsigned int length);
    void publishPlayMessage(const String& dirPath, const String& fileName);

    PlaybackManager* _player = nullptr;

    // Network
    WiFiClient _wifi;
    PubSubClient _mqtt;

    // Identity (zur Ignore-Sperre von eigenen Messages)
    String _selfId;
    unsigned long _lastReconnectAttemptMs = 0;
};

