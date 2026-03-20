#include <Arduino.h>
#include "PlaybackManager.h"
#include "MqttSyncManager.h"
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

    const String& dirPath = directoryList[_currentDirectoryIndex];
    playFileFromDirectoryPath(dirPath, _introFileName, true);
}

void PlaybackManager::playRandomFile() {
    if (_currentDirectoryIndex < 0 || currentMp3Files.empty()) return;

    int randomIndex = random(currentMp3Files.size());
    const String& dirPath = directoryList[_currentDirectoryIndex];
    playFileFromDirectoryPath(dirPath, currentMp3Files[randomIndex], true);
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
    if (!dir) return;

    // Dateien im Ordner enumerieren
    while (true) {
        File file = dir.openNextFile();
        if (!file) break;

        String name = file.name();
        if (name.endsWith(".mp3") && name != String(_introFileName)) {
            currentMp3Files.push_back(name);
        }
        file.close();
    }
    dir.close();
}

void PlaybackManager::findMp3Directories(File dir) {
    if (!dir) return;

    // Erwartung laut README: MP3-Dateien liegen in Unterordnern auf der SD-Karte.
    // Pro Unterordner mit mindestens einer ".mp3" Datei wird ein Sound-Set aufgenommen.
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        if (!entry.isDirectory()) {
            entry.close();
            continue;
        }

        String dirName = entry.name(); // meist ohne führenden Slash
        String dirPath = "/" + dirName;

        bool hasMp3 = false;
        while (true) {
            File f = entry.openNextFile();
            if (!f) break;

            String name = f.name();
            if (name.endsWith(".mp3")) {
                hasMp3 = true;
                f.close();
                break;
            }
            f.close();
        }

        entry.close();

        if (hasMp3) {
            directoryList.push_back(dirPath);
        }
    }
}

bool PlaybackManager::setCurrentDirectoryByPath(const String& dirPath) {
    for (int i = 0; i < (int)directoryList.size(); i++) {
        if (directoryList[i] == dirPath) {
            _currentDirectoryIndex = i;
            loadFilesFromCurrentDirectory();
            return true;
        }
    }
    return false;
}

bool PlaybackManager::playFileFromDirectoryPath(const String& dirPath, const String& fileName, bool notify) {
    if (dirPath.length() == 0 || fileName.length() == 0) return false;

    // Remote-Play soll die interne Directory-Auswahl konsistent halten.
    setCurrentDirectoryByPath(dirPath);

    String filePath = dirPath;
    if (!filePath.endsWith("/")) filePath += "/";
    filePath += fileName;

    Serial.println("Playing MP3: " + filePath);
    return _playFilePath(filePath, dirPath, fileName, notify);
}

bool PlaybackManager::_playFilePath(const String& filePath, const String& dirPath, const String& fileName, bool notify) {
    if (_isPlaying) {
        audio.stopSong();
    }

    bool ok = audio.connecttoFS(SD, filePath.c_str());
    if (!ok) {
        Serial.println("-> Could not play file.");
        return false;
    }

    _isPlaying = true;
    _isPlayingIntro = (fileName == String(_introFileName));

    if (notify && _mqttSync) {
        _mqttSync->notifyLocalPlay(dirPath, fileName);
    }

    return true;
}

