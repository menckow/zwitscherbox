#include "config.h"
#include "HardwareManager.h"
#include "PlaybackManager.h"
#include "StateManager.h"

// Erstelle die globalen Objekte
HardwareManager hardware(POT_PIN, PIR_PIN, BUTTON_PIN);
PlaybackManager player;
StateManager    stateMgr;

// Globale Zustandsvariablen für die Hauptlogik
unsigned long lastActivityTime = 0;
unsigned long playbackStartTime = 0;
bool inPlaybackSession = false;

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- Modular MP3 Player Starting ---");
    randomSeed(analogRead(0));

    // SD-Karte initialisieren (wird für Player und Config benötigt)
    SPI.begin(); // oder spi_onboardSD->begin(); je nach Board
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD Card Mount Failed!");
        while(1);
    }
    
    // Initialisiere die Manager
    hardware.begin();
    player.begin(I2S_BCLK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN);
    stateMgr.begin();

    // Lade den letzten Zustand
    int lastDirIndex, lastVolume;
    stateMgr.loadState(lastDirIndex, lastVolume);
    
    // Setze den Zustand im Player
    if (lastDirIndex < player.getDirectoryCount()) {
        player.setCurrentDirectoryIndex(lastDirIndex);
    }
    player.setVolume(lastVolume);
    
    lastActivityTime = millis();
    Serial.println("Setup complete. Waiting for activity.");
}

void loop() {
    // 1. Alle Manager-Objekte updaten
    hardware.update();
    player.update();

    // 2. Status abfragen und Logik ausführen
    bool isPirActive = hardware.isPirActive();
    bool isPlaying = player.isPlaying();

    // Aktivitäts-Timer aktualisieren
    if (isPirActive || isPlaying) {
        lastActivityTime = millis();
    }
    
    // Lautstärke setzen
    player.setVolume(hardware.getVolume());

    // Knopf gedrückt?
    if (hardware.wasButtonPressed()) {
        lastActivityTime = millis();
        inPlaybackSession = false;
        player.stop("Button");
        if (player.nextDirectory()) {
            player.playIntro();
        }
    }
    
    // PIR getriggert?
    if (isPirActive && !isPlaying && !inPlaybackSession) {
        Serial.println("PIR Trigger!");
        inPlaybackSession = true;
        playbackStartTime = millis();
        player.playRandomFile();
    }
    
    // Ende der Wiedergabe-Session (wird durch EOF oder Timeout beendet)
    if (!isPlaying && inPlaybackSession) {
        inPlaybackSession = false; 
    }

    // Wiedergabe-Timeout
    if (inPlaybackSession && (millis() - playbackStartTime > MAX_PLAYBACK_DURATION)) {
        player.stop("Timeout");
        inPlaybackSession = false;
    }
    
    // 3. Deep Sleep prüfen (nur wenn nichts passiert)
    if (!isPlaying && !inPlaybackSession) {
        stateMgr.saveState(player.getCurrentDirectoryIndex(), hardware.getVolume());
        stateMgr.checkAndEnterDeepSleep(lastActivityTime, PIR_PIN);
    }
    
    vTaskDelay(10); // Kurze Pause für den Task Scheduler
}