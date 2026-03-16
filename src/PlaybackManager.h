// ---------------------------------------------------------------- //
//                  DATEI: PlaybackManager.h                        //
// ---------------------------------------------------------------- //
#include "Audio.h"
#include <FS.h>
#include <SD.h>
#include <vector>

class PlaybackManager {
public:
    PlaybackManager();
    void begin(int bclk, int lrclk, int dout);
    void update(); // Muss in loop() aufgerufen werden

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
    
    // Callback-Handler
    void onPlaybackFinished();

private:
    void findMp3Directories(File dir);
    void loadFilesFromCurrentDirectory();

    Audio audio;
    std::vector<String> directoryList;
    std::vector<String> currentMp3Files;
    int _currentDirectoryIndex = -1;
    bool _isPlaying = false;
    bool _isPlayingIntro = false;
    const char* _introFileName = "intro.mp3";
};