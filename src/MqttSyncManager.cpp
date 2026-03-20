#include "MqttSyncManager.h"

#include "PlaybackManager.h"
#include "config.h"

#include <string.h>

static MqttSyncManager* g_mqttSync = nullptr;

static void mqtt_message_callback(char* topic, uint8_t* payload, unsigned int length) {
    if (g_mqttSync) {
        g_mqttSync->onMqttMessage(topic, payload, length);
    }
}

MqttSyncManager::MqttSyncManager(PlaybackManager* player)
    : _player(player), _mqtt(_wifi) {
    g_mqttSync = this;
}

void MqttSyncManager::begin() {
    if (!isEnabled()) return;

    Serial.println("--- MQTT Sync starting ---");
    ensureWifi();

    // Selbst-Identität (wird auch im Payload mitgeschickt, damit wir eigene Messages ignorieren können)
    String mac = WiFi.macAddress(); // z.B. "AA:BB:CC:DD:EE:FF"
    mac.replace(":", "");
    if (mac.length() > 6) {
        mac = mac.substring(mac.length() - 6);
    }
    _selfId = String(MQTT_CLIENT_ID_BASE) + "_" + mac;

    _mqtt.setCallback(mqtt_message_callback);
    _mqtt.setServer(MQTT_SERVER, MQTT_PORT);

    ensureMqttConnected();
}

void MqttSyncManager::ensureWifi() {
    if (strlen(WIFI_SSID) == 0) {
        Serial.println("MQTT_INTEGRATION aktiv, aber WIFI_SSID ist leer. MQTT wird nicht verbinden.");
        return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    const unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < 20000UL) {
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected, IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi not connected (timeout). MQTT connection will retry in background.");
    }
}

void MqttSyncManager::ensureMqttConnected() {
    if (!isEnabled()) return;
    if (_mqtt.connected()) return;

    // Nur alle X ms erneut versuchen (damit es nicht spamt)
    const unsigned long now = millis();
    if (now - _lastReconnectAttemptMs < 5000UL) return;
    _lastReconnectAttemptMs = now;

    if (WiFi.status() != WL_CONNECTED) {
        ensureWifi();
        return;
    }

    bool ok = false;
    if (strlen(MQTT_USER) > 0) {
        ok = _mqtt.connect(_selfId.c_str(), MQTT_USER, MQTT_PASS);
    } else {
        ok = _mqtt.connect(_selfId.c_str());
    }

    if (ok) {
        Serial.println("MQTT connected.");
        _mqtt.subscribe(MQTT_SHARED_PLAY_TOPIC);
    } else {
        Serial.print("MQTT connect failed, state=");
        Serial.println(_mqtt.state());
    }
}

void MqttSyncManager::update() {
    if (!isEnabled()) return;

    if (!_mqtt.connected()) {
        ensureMqttConnected();
        return;
    }

    _mqtt.loop();
}

void MqttSyncManager::notifyLocalPlay(const String& dirPath, const String& fileName) {
    if (!isEnabled()) return;
    if (!_mqtt.connected()) {
        ensureMqttConnected();
        if (!_mqtt.connected()) return;
    }

    publishPlayMessage(dirPath, fileName);
}

void MqttSyncManager::publishPlayMessage(const String& dirPath, const String& fileName) {
    // Payload: <senderId>|<dirPath>|<fileName>
    String payload = _selfId + "|" + dirPath + "|" + fileName;
    bool ok = _mqtt.publish(MQTT_SHARED_PLAY_TOPIC, payload.c_str());
    if (ok) {
        Serial.print("MQTT publish play: ");
        Serial.println(payload);
    } else {
        Serial.println("MQTT publish failed.");
    }
}

void MqttSyncManager::onMqttMessage(char* /*topic*/, uint8_t* payload, unsigned int length) {
    if (!isEnabled()) return;
    if (!_player) return;

    String msg;
    msg.reserve(length + 1);
    for (unsigned int i = 0; i < length; i++) {
        msg += static_cast<char>(payload[i]);
    }

    // Erwartetes Format: senderId|dirPath|fileName
    int p1 = msg.indexOf('|');
    int p2 = (p1 >= 0) ? msg.indexOf('|', p1 + 1) : -1;
    if (p1 < 0 || p2 < 0) {
        Serial.println("MQTT play payload invalid.");
        return;
    }

    String senderId = msg.substring(0, p1);
    if (senderId == _selfId) return; // Self-trigger ignorieren

    String dirPath = msg.substring(p1 + 1, p2);
    String fileName = msg.substring(p2 + 1);

    Serial.print("MQTT received play: ");
    Serial.println(msg);

    // Remote-Play => nicht erneut publishen
    _player->playFileFromDirectoryPath(dirPath, fileName, false);
}

