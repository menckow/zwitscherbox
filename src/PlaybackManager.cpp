#include <Arduino.h>
#include "PlaybackManager.h"
#include "config.h"

// Globaler Zeiger für den Callback
PlaybackManager* globalPlaybackManager = nullptr;

// Globale Callback-Funktion, die die Klassenmethode aufruft
void audio_eof_mp3(const char *info) {
    if (globalPlaybackManager) {
        globalPlaybackManager->onPlaybackFinished();
    }
}

PlaybackManager::PlaybackManager() {
    globalPlaybackManager = this; // Setze den globalen Zeiger auf diese Instanz
}

void PlaybackManager::begin(int bclk, int lrclk, int dout) {
    audio.setPinout(bclk, lrclk, dout);
    
    Serial.println("Scanning SD card for MP3 directories...");
    File root = SD.open("/");
    if (root) {
        findMp3Directories(root);
        root.close();
        Serial.printf("Found %d directories.\n", directoryList.size());
    }
}

void PlaybackManager::update() {
    audio.loop();
}

void PlaybackManager::setVolume(int volume) {
    audio.setVolume(volume);
}

void PlaybackManager::onPlaybackFinished() {
    _isPlaying = false;
    _isPlayingIntro = false;
    Serial.println("Playback finished (EOF).");
}

bool PlaybackManager::nextDirectory() {
    if (directoryList.empty()) return false;
    
    _currentDirectoryIndex = (_currentDirectoryIndex + 1) % directoryList.size();
    loadFilesFromCurrentDirectory();
    Serial.println("Switched to directory: " + directoryList[_currentDirectoryIndex]);
    return true;
}

void PlaybackManager::playIntro() {
    if (_currentDirectoryIndex < 0) return;
    
    String introPath = directoryList[_currentDirectoryIndex] + "/" + _introFileName;
    Serial.println("Playing intro: " + introPath);
    if (audio.connecttoFS(SD, introPath.c_str())) {
        _isPlaying = true;
        _isPlayingIntro = true;
    } else {
        Serial.println("-> Intro file not found.");
    }
}

void PlaybackManager::playRandomFile() {
    if (_currentDirectoryIndex < 0 || currentMp3Files.empty()) return;

    int randomIndex = random(currentMp3Files.size());
    String filePath = directoryList[_currentDirectoryIndex] + "/" + currentMp3Files[randomIndex];
    Serial.println("Playing random file: " + filePath);
    if (audio.connecttoFS(SD, filePath.c_str())) {
        _isPlaying = true;
    } else {
        Serial.println("-> Could not play file.");
    }
}

void PlaybackManager::stop(const char* reason) {
    if (_isPlaying) {
        audio.stopSong();
        _isPlaying = false;
        _isPlayingIntro = false;
        Serial.printf("Playback stopped. Reason: %s\n", reason);
    }
}

// Private Methoden
void PlaybackManager::loadFilesFromCurrentDirectory() {
    currentMp3Files.clear();
    if (_currentDirectoryIndex < 0) return;

    String currentDirPath = directoryList[_currentDirectoryIndex];
    File dir = SD.open(currentDirPath);
    // ... (Logik zum Laden der Dateien wie im Originalcode)
    dir.close();
}

void PlaybackManager::findMp3Directories(File dir) {
    // ... (Logik zum Finden der Verzeichnisse wie im Originalcode)
}

