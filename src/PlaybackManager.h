// ---------------------------------------------------------------- //
//                  DATEI: PlaybackManager.h                        //
// ---------------------------------------------------------------- //
#include "Audio.h"
#include <FS.h>
#include <SD.h>
#include <vector>

class MqttSyncManager;

class PlaybackManager {
public:
    PlaybackManager();
    void begin(int bclk, int lrclk, int dout);
    void update(); // Muss in loop() aufgerufen werden

    // Wird nur für MQTT-Play-Sync benötigt.
    void setMqttSync(MqttSyncManager* mqttSync) { _mqttSync = mqttSync; }

    void setVolume(int volume);
    bool nextDirectory();
    void playIntro();
    void playRandomFile();
    void stop(const char* reason);
    
    // Status abfragen
    bool isPlaying() { return _isPlaying; }
    int getCurrentDirectoryIndex() { return _currentDirectoryIndex; }
    void setCurrentDirectoryIndex(int index) { _currentDirectoryIndex = index; loadFilesFromCurrentDirectory(); }
    int getDirectoryCount() { return directoryList.size(); }

    // Remote-Play: kann anhand des Verzeichnis-Pfads den internen Index setzen und dann spielen.
    bool setCurrentDirectoryByPath(const String& dirPath);
    bool playFileFromDirectoryPath(const String& dirPath, const String& fileName, bool notify = false);
    
    // Callback-Handler
    void onPlaybackFinished();

private:
    void findMp3Directories(File dir);
    void loadFilesFromCurrentDirectory();

    bool _playFilePath(const String& filePath, const String& dirPath, const String& fileName, bool notify);

    Audio audio;
    std::vector<String> directoryList;
    std::vector<String> currentMp3Files;
    int _currentDirectoryIndex = -1;
    bool _isPlaying = false;
    bool _isPlayingIntro = false;
    const char* _introFileName = "intro.mp3";

    MqttSyncManager* _mqttSync = nullptr;
};