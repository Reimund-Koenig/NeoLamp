#ifndef CLOCKTIME
#define CLOCKTIME
#include <Arduino.h>

class Clocktime {
  public:
    Clocktime();
    void print();
    String getTimeString();
    bool setTime(String timestring);
    bool setTime(int hour, int minutes);

  private:
    int minutes;
    int hours;
};
#endif // CLOCKTIME