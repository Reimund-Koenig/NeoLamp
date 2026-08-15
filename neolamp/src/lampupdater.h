#ifndef LAMPUPDATER
#define LAMPUPDATER
#include "lampfilesystem.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

static void update_started();
static void update_finished();
static void update_progress(int cur, int total);
static void update_error(int err);

class LampUpdater {
  public:
    LampUpdater(LampFileSystem *lfs);

  private:
    LampFileSystem *lfs;
    String getNewVersionString();
    String newVersionString;
    bool update();
};

#endif // LAMPUPDATER
