#include "lampupdater.h"
#include "constants/settings.h"

#define HTTP_VERSION "newestLampVersion.info"
#define HTTP_SERVER_URL "http://reimund-koenig.de/data/neolamp/"
#define CURRENT_VERSION_FS "current_version.txt" // do not change

LampUpdater::LampUpdater(LampFileSystem *lfs) {
    this->lfs = lfs;
    newVersionString = getNewVersionString();
    if(newVersionString == "") return;
    String currentVersion = lfs->read_file(CURRENT_VERSION_FS);
    if(currentVersion == "" || currentVersion == NULL) {
        currentVersion = FILENAME_VERSION;
        lfs->write_file(CURRENT_VERSION_FS, currentVersion.c_str());
    }
    Serial.print("Current Version: ");
    Serial.println(currentVersion);
    Serial.print("Web Version: ");
    Serial.println(newVersionString);
    if(currentVersion == newVersionString) return;
    Serial.println("Start Software Update");
    ESPhttpUpdate.rebootOnUpdate(false);
    // ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); // Optional
    if(update()) {
        lfs->write_file(CURRENT_VERSION_FS, newVersionString.c_str());
        ESP.restart();
    }
}

String LampUpdater::getNewVersionString() {
    HTTPClient http;
    WiFiClient client;
    Serial.print("[HTTP] begin...\n");
    String payload = "";
    if(http.begin(client, String(HTTP_SERVER_URL) + String(HTTP_VERSION))) {
        // HTTP
        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been
            // handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK ||
               httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payload = http.getString();
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n",
                          http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("[HTTP] Unable to connect");
    }
    return payload;
}

bool LampUpdater::update() {
    const String HTTP_URL = String(HTTP_SERVER_URL) + newVersionString;
    Serial.println(HTTP_URL);
    WiFiClient client;
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, HTTP_URL);
    switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
                      ESPhttpUpdate.getLastError(),
                      ESPhttpUpdate.getLastErrorString().c_str());
        return false;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        return false;
    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        return true;
    }
    return false;
}

static void update_finished() {
    Serial.println("CALLBACK:  HTTP update process finished");
}

static void update_started() {
    Serial.println("CALLBACK:  HTTP update process started");
}

static void update_progress(int cur, int total) {
    Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur,
                  total);
}

static void update_error(int err) {
    Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
