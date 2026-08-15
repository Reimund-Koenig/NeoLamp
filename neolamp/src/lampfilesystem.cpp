#include "lampfilesystem.h"

LampFileSystem::LampFileSystem() {
    const uint8_t maxAttempts = 10;

    for(uint8_t attempt = 0; attempt < maxAttempts; ++attempt) {
        if(SPIFFS.begin()) { return; }

        Serial.println("SPIFFS mount failed, retrying...");
        delay(100);
        yield();
    }

    Serial.println("SPIFFS mount failed permanently; continuing without "
                   "filesystem access.");
}

void LampFileSystem::write_file(const char *path, const char *message) {
    write(SPIFFS, path, message);
}
void LampFileSystem::write(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, "w");
    if(!file) { return; }
    file.print(message);
    file.close();
}

String LampFileSystem::read_file(const char *path) {
    return read(SPIFFS, path);
}

String LampFileSystem::read(fs::FS &fs, const char *path) {
    File file = fs.open(path, "r");
    if(!file || file.isDirectory()) { return String(); }
    String fileContent;
    while(file.available()) {
        fileContent += String((char)file.read());
    }
    file.close();
    return fileContent;
}
