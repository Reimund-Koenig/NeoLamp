#include "lampfilesystem.h"

LampFileSystem::LampFileSystem() {
    while(!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
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
