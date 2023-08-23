#ifndef LAMPFILESYSTEM
#define LAMPFILESYSTEM
#include <FS.h>
#include <Hash.h>

class LampFileSystem {
  public:
    LampFileSystem();
    void init();
    void write_file(const char *path, const char *message);
    String read_file(const char *path);

  private:
    void write(fs::FS &fs, const char *path, const char *message);
    String read(fs::FS &fs, const char *path);
};

#endif // LAMPFILESYSTEM
