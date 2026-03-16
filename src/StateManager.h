// ---------------------------------------------------------------- //
//                  DATEI: StateManager.h                           //
// ---------------------------------------------------------------- //
#include <Preferences.h>
#include "esp_sleep.h"

class StateManager {
public:
    StateManager();
    void begin();
    void saveState(int dirIndex, int volume);
    void loadState(int &dirIndex, int &volume);
    void checkAndEnterDeepSleep(unsigned long lastActivityTime, int pirPin);

private:
    Preferences preferences;
};
